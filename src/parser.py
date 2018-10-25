#!/usr/bin/python

from __future__ import print_function
from ROOT import TCanvas, TGraph, TVirtualFFT, TH1F, TPad
from ROOT import gROOT, gStyle
from math import sin, floor
from array import array
import serial
import time
import numpy as np

def main():
    graph = EEG_Graph(100)
    st = time.time()

    while True:
        delta = time.time() - st
        delta *= 3
        graph.append(delta, sin(delta * 5) * 500 + (sin(delta * 20) * 200))
        #time.sleep(0.001)

    """
    port = '/dev/ttyUSB0'
    baudrate = 115200
    data_array = []
    t = 0.03
    print('Listening on port:', port, 'at baudrate:', baudrate)
    with serial.Serial(port, baudrate) as ser:
        while True:
            ser.reset_input_buffer()
            data = ser.readline().decode('UTF-8').split(':')
            if data[0] == 'd':
                data_array.append(int(data[1]))
                graph.append(time.time() - st, int(data[1]))
                #print(data[1])
                #print(ser.inWaiting())
            elif data[0] == 'r':
                print("r: %2i" % (len(data),), end="")
                if data[1] == '2':
                    if int(data[2]) < 30:
                        graph.setQuality(True)
                    else:
                        graph.setQuality(False)
                    print(' signal quality %i' % (int(data[2]),))
                else:
                    print(' got an error')
            #time.sleep(0.03)
            t += 0.03
    """


    """
    for i in range(1000):
        time.sleep(0.1)
        n += 1
        gr.Set(n)
        gr.SetPoint(n - 1, 0.1 * n, 4)
        # TCanvas.Update() draws the frame, after which one can change it
        c1.Update()
        c1.GetFrame().SetFillColor( 21 )
        c1.GetFrame().SetBorderSize( 12 )
        c1.Modified()
        c1.Update()
    """

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

        #self.canvas.SetFillColor(18)

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
        #self.fft = self.graph.GetHistogram().FFT(self.fft, "MAG R2C RE")
        self.fft.SetTitle("EEG FFT")
        #self.fft.SetMaximum(100)
        #labels = ["1 Hz", "50 Hz", "100 Hz"]
        self.fft.Fill("1 Hz", 0)
        self.fft.Fill("2 Hz", 0)
        #self.fft.Fill(labels[1], 160)
        #self.fft.Fill(labels[2], 80)
        #self.fft.Fill(labels[2], 400)
        self.fft.SetMinimum(0)
        self.fft.SetMaximum(100000)
        #self.fft.SetLineColor(2)
        self.fft.SetFillStyle(3001)
        self.fft.SetFillColor(30)
        #self.fft.Draw("HIST")
        self.fft.Draw("B HIST")
        self.ampmax = 100000

        self.fft.SetStats(False)
        self.fft.GetXaxis().SetLabelSize(0.05)
        self.fft.GetYaxis().SetLabelSize(0.05)

    def setQuality(self, good):
        self.gq = good

    def append(self, timep, num):
        #print("Adding: %f -> %i" % (time, num))
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

        #self.fft = self.graph.GetHistogram().FFT(self.fft, "MAG R2C RE")

        self.data_fft = np.abs(np.fft.fft(self.data))
        #print(len(self.data_fft))
        #print(self.data_fft[1])
        self.fft.Reset()
        if len(self.data_fft) > 256:
            delta = self.data_time[-1] - self.data_time[0]
            for i in range(50):
                #index = i * delta
                #print("index %i" % (index,))
                amp = np.sum(self.data_fft[round(i * delta):round(i * delta + delta)])
                #print("    %i Hz" % (i + 1,))
                self.fft.Fill("%i Hz" % (i + 1,), amp)
                if amp > self.ampmax:
                    self.ampmax = amp
                    self.fft.SetMaximum(amp)

            #print(delta)

            #print("The frequency is {} Hz".format(np.argmax(self.data_fft)))
        #print(self.data_fft)

        self.update()

    def update(self):
        # TCanvas.Update() draws the frame, after which one can change it
        #self.canvas.Update()
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
