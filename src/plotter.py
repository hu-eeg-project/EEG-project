#!/usr/bin/python

import sys
import matplotlib.pyplot as plt
import os
from math import ceil

def main():
    if len(sys.argv) < 2:
        print("Need file to plot as argument")
        return

    plots = []

    for arg in sys.argv[1:]:
        if os.path.isdir(arg):
            l = os.listdir(arg)
            for plot in l:
                if plot.endswith(".eeg"):
                    plots += [arg + "/" + plot]
        else:
            plots += [arg]

    y = 2
    x = ceil(len(plots)/y)
    fig, ax = plt.subplots(x, y, sharex=True)

    xbuf = 0
    ybuf = 0

    plot_count = 0

    for plot in plots:
        f = open(plot, "r")
        lines = f.read().splitlines()
        f.close()

        time_array = []
        data_array = []

        offset = -1
        count = 0

        line_color = 'green'

        for line in lines:
            split_line = line.split(',')
            if (split_line[0] == "metadata"):
                if (split_line[1] == 'square'):
                    line_color = 'red'
                elif (split_line[1] == 'circle'):
                    line_color = 'blue'
                continue
            if offset < 0:
                offset = float(split_line[0])
            time_array.append(float(split_line[0]) - offset)
            data_array.append(float(split_line[1]))
            if float(split_line[1]) > 3800 or float(split_line[1]) < 200:
                #count += 1
                pass

        if count < len(lines) / 3:
            ax[xbuf, ybuf].plot(time_array, data_array, color=line_color)
            plot_count += 1
            xbuf += 1
            if xbuf >= x:
                xbuf = 0
                ybuf += 1

    plt.xlabel("Time")
    plt.ylabel("Amplitude")
    plt.show()

if __name__ == "__main__":
    main()
