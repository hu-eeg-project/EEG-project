import serial
from math import sin, pi
import time
import threading

class Force_io:
    def __init__(self, value_q, time_q):
        self.value_q = value_q
        self.time_q = time_q
        self.signal_thread = None
        self.io_thread = None
        self.signal_stop = threading.Event()
        self.serial_stop = threading.Event()
        
    def start_serial(self, port, baudrate):
        self.serial_thread = threading.Thread(target=self.serial_thread,
                                              args=(self.value_q,self.time_q,port,baudrate,self.serial_stop))
        self.serial_thread.setDaemon(True)
        self.serial_thread.start()

    def stop_serial(self):
        if self.serial_thread != None:
            self.serial_stop.set()
            self.serial_stop.join()

    def start_test_signal(self, f, fs):
        self.signal_thread = threading.Thread(target=self.test_signal_thread,
                                              args=(self.value_q,self.time_q,f,fs,self.signal_stop))
        self.signal_thread.setDaemon(True)
        self.signal_thread.start()
        
    def stop_test_signal(self):
        if self.signal_thread != None:
            self.signal_stop.set()
            self.signal_thread.join()

    def test_signal_thread(self, value_q, time_q, f, fs, stop_evt):
        st = time.time()
        t = 1.0 / fs
        while not stop_evt.is_set():
            result = 0
            nt = time.time()
            for i in f:
                result += i[1] * sin(i[0] * 2.0 * pi * (nt-st))
            value_q.append(result)
            time_q.append(nt-st)
            time.sleep(t)

    def serial_thread(self, value_q, time_q, port, baudrate, serial_stop):
        st = time.time()
        with serial.Serial(port, baudrate) as ser:
            while not serial_stop.is_set():
                data = ser.readline()
                if len(data):
                    if data[0] == ord(b'd'):
                        data = data.decode('UTF-8').split(':')
                        value_q.append(int(data[1]))
                        time_q.append(time.time()-st)
                    
    
        
def main():
    from math import sin, pi, sqrt
    import matplotlib.pyplot as plt
    import collections
    import numpy as np
    import scipy.fftpack
    
    #pass array of frequencies
    #index 0 is frequency, index 1 is amplitude
    f = [(1, 1), (100, 1)]
    fs = 1000
    n = 512
    t = 1.0 / fs
    x = np.linspace(0.0, n*t, n)
    t = 1.0 / fs

    
    value_q = collections.deque(maxlen=n)
    time_q = collections.deque(maxlen=n)

    io = Force_io(value_q, time_q)
    io.start_test_signal(f, fs)

    xf = np.linspace(0.0, 1.0/(2.0*t), int(n/2))
    
    while True:
        if len(value_q) == n:
            value_cpy = value_q.copy()
            time_cpy = time_q.copy()
            value_q.clear()
            time_q.clear()
            fft = scipy.fftpack.fft(value_cpy)
            plt.plot(xf[1:], 2.0/n * np.abs(fft[0:int(n/2)])[1:])
            plt.show()
            
    io.stop_test_signal()

if __name__ == '__main__':
    main()
        
