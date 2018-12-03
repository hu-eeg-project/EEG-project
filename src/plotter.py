#!/usr/bin/python

import sys
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) != 2:
        print("Need file to plot as argument")
        return

    f = open(sys.argv[1], "r")
    lines = f.read().splitlines()
    f.close()

    time_array = []
    data_array = []

    for line in lines:
        split_line = line.split(',')
        time_array.append(float(split_line[0]))
        data_array.append(float(split_line[1]))

    plt.plot(time_array, data_array)
    plt.xlabel("Time")
    plt.ylabel("Amplitude")
    plt.show()

if __name__ == "__main__":
    main()
