#!/usr/bin/python

from __future__ import print_function
from ROOT import TCanvas, TGraph, TVirtualFFT, TH1F, TPad
from ROOT import gROOT, gStyle
from math import sin, floor, pi
from array import array
import serial
import time
import numpy as np
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
    from math import sin, pi, tan, cos
    import matplotlib.pyplot as plt
    import collections
    import numpy as np

    #pass array of frequencies
    #index 0 is frequency, index 1 is amplitude
    f = [(1, 1), (10, 0.3)]
    fs = 1000
    n = 512
    t = 1.0 / fs
    x = np.linspace(0.0, n*t, n)

    value_q = collections.deque(maxlen=n)
    time_q = collections.deque(maxlen=n)

    graph = EEG_Graph(100)

    io = Force_io(value_q, time_q)
    io.start_test_signal(f, fs)

    #st = time.time()

    while True:
        value_cpy = value_q.copy()
        time_cpy = time_q.copy()
        value_q.clear()
        time_q.clear()

        for i in range(len(value_cpy)):
            graph.append(time_cpy[i], value_cpy[i] * 1000)

    io.stop_test_signal()

class EEG_Graph(object):
    def __init__(self, maxpoints=60):
        self.maxpoints = maxpoints
        self.gq = False
        self.canvas = TCanvas('c1', 'A Simple Graph Example', 200, 10, 700, 500)
        self.canvas_1 = TPad("c1_1", "c1_1", 0.01, 0.67, 0.99, 0.99)
        self.canvas_2 = TPad("c1_2", "c1_2", 0.01, 0.01, 0.99, 0.66)

        self.canvas_1.SetGrid()
        self.canvas_2.SetGridx()

        self.canvas_1.Draw();
        self.canvas_2.Draw();

        self.data = [0]
        self.data_time = [time.time()]

        n = 1
        x = array('d')
        y = array('d')
        x.append(0)
        y.append(0)

        self.canvas_1.cd()
        self.graph = TGraph(n, x, y)
        self.graph.SetLineColor(2)
        self.graph.SetLineWidth(4)
        self.graph.SetMarkerColor(4)
        self.graph.SetMarkerStyle(2)
        self.graph.SetTitle('EEG Signal')
        self.graph.GetXaxis().SetTitle('Time')
        self.graph.GetYaxis().SetTitle('Amplitude')
        self.graph.GetYaxis().SetRangeUser(-2000,2000);
        self.graph.Draw('ACP')

        self.canvas_2.cd()
        TVirtualFFT.SetTransform(0)
        self.fft = TH1F("fft", "eeg_fft", 3, 0, 3)
        self.fft.SetTitle("EEG FFT")
        self.fft.Fill("1 Hz", 0)
        self.fft.Fill("2 Hz", 0)
        self.fft.SetMinimum(0)
        self.fft.SetMaximum(100000)
        self.fft.SetFillStyle(3001)
        self.fft.SetFillColor(30)
        self.fft.Draw("B HIST")
        self.ampmax = 100000

        self.fft.SetStats(False)
        self.fft.GetXaxis().SetLabelSize(0.05)
        self.fft.GetYaxis().SetLabelSize(0.05)

    def setQuality(self, good):
        self.gq = good

    def append(self, timep, num):
        n = self.graph.GetN()
        if len(self.data) < 2048:
            self.data = self.data + [num]
            self.data_time = self.data_time + [time.time()]
        else:
            self.data = self.data[1:] + [num]
            self.data_time = self.data_time[1:] + [time.time()]

        if n < self.maxpoints:
            self.graph.Set(n + 1)
            self.graph.SetPoint(n, timep, num)
        else:
            self.graph.RemovePoint(0)
            self.graph.Set(n)
            self.graph.SetPoint(n - 1, timep, num)

        self.data_fft = np.abs(np.fft.fft(self.data))
        self.fft.Reset()
        if len(self.data_fft) > 256:
            delta = self.data_time[-1] - self.data_time[0]
            for i in range(50):
                amp = np.sum(self.data_fft[round(i * delta):round(i * delta + delta)])
                self.fft.Fill("%i Hz" % (i + 1,), amp)
                if amp > self.ampmax:
                    self.ampmax = amp
                    self.fft.SetMaximum(amp)

        self.update()

    def update(self):
        self.canvas_1.cd()
        if self.gq:
            self.canvas_1.GetFrame().SetFillColor(30)
        else:
            self.canvas_1.GetFrame().SetFillColor(46)
        self.canvas.GetFrame().SetBorderSize(12)
        self.graph.GetYaxis().SetRangeUser(-2000,2000);

        self.canvas_2.Modified()
        self.canvas.Modified()
        self.canvas.Update()

if __name__ == "__main__":
    main()
