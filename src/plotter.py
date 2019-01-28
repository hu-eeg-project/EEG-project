#!/usr/bin/python

import sys
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import os
from math import ceil, floor

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
    #fig, ax = plt.subplots(x, y, sharex=True)

    xbuf = 0
    ybuf = 0

    plot_count = 0

    all_lines = []

    longest_len = 0
    longest_time_array = []
    max_value = 0
    min_value = 0
    
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
            if line_color == 'blue':
                time_array.append(float(split_line[0]) - offset)
                data_array.append(float(split_line[1]))
                
            if float(split_line[1]) > 3800 or float(split_line[1]) < 200:
                pass
                #count += 1
        all_lines.append(data_array)

        if(len(data_array)):
            max_val = max(data_array)
            if max_val > max_value:
                max_value = max_val

            min_val = min(data_array)
            if min_val < min_value:
                min_value = min_val
            
        if len(data_array) > longest_len:
            longest_len = len(data_array)
            longest_time_array = time_array

        if count < len(lines) / 3:
            #ax[xbuf, ybuf].plot(time_array, data_array, color=line_color)
            plt.plot(time_array, data_array, color=line_color)
            plot_count += 1
            xbuf += 1
            if xbuf >= x:
                xbuf = 0
                ybuf += 1

    print(max_value, min_value)
                
    points_list = []
    
    totals = []
    index = 0
    buffer = []
    for num in range(longest_len):
        buffer = []
        for line in all_lines:
            if len(line) > num:
                buffer.append(line[num])
        points_list.append(buffer)

    averages = []
    medians = []
    q3 = []
    for points in points_list:
        averages.append(sum(points)/len(points))

    for points in points_list:
        points.sort()
        length = len(points)
        
        left = points[floor(length / 2)]
        right = points[ceil(length / 2)]
        medians.append((left + right)/2)

        lower = length * 3 / 4
        left = points[floor(lower)]

        if ceil(lower) >= length:
            right = points[length-1]
        else:
            right = points[ceil(lower)]
        
        
        
        q3.append((left + right)/2)
        
            
    plt.plot(longest_time_array, averages, color="red", linewidth = 3)
    plt.plot(longest_time_array, medians, color="orange", linewidth = 3)
    plt.plot(longest_time_array, q3, color="#18E800", linewidth = 3)

    legend_elements = [Line2D([0], [0], color="red", lw=3, label="gemiddelde"),
                       Line2D([0], [0], color="orange", lw=3, label="mediaan"),
                       Line2D([0], [0], color="#18E800", lw=3, label="q3")]
    
    plt.legend(handles=legend_elements, loc='lower right')
    plt.ylim(min_value, max_value)
    plt.xlabel("Time")
    plt.ylabel("Amplitude")
    plt.show()

if __name__ == "__main__":
    main()
