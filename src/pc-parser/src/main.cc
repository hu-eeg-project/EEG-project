/*
 * \file main.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */

#include <stdio.h>
#include <unistd.h>

#include "serial.hh"
#include "eeg-graph.hh"
#include "rolling-array.hh"
#include "wave-generator.hh"

#include <thread>
#include <math.h>
#include <time.h>
#include <chrono>

void dataThread(ArrayPair<RollingArray<Double_t>,
                RollingArray<Double_t>>* array)
{
    Frequency_t frequencies[2] = {{500, 1}, {300, 50}};
    WaveGenerator wave(frequencies, 2, array);

    while (true) {
        array->lock();
        wave.genSample();
        array->unlock();

        usleep(100);
    }
}

int main(int argc, char* argv[])
{
    EEGGraph eeg(&argc, argv);

    RollingArray<Double_t> data_array(4096);
    RollingArray<Double_t> time_array(4096);
    ArrayPair<RollingArray<Double_t>, RollingArray<Double_t>> data(data_array, time_array);

    std::thread data_thread(dataThread, &data);

    Double_t data_array_copy[4096];
    Double_t time_array_copy[4096];

    bool looping = true;
    while (looping) {
        data.lock();
        int size = data.array1.getSize();
        for (int i = 0; i < size; i++) {
            data_array_copy[i] = data.array1.getData()[i];
            time_array_copy[i] = data.array2.getData()[i];
        }
        data.unlock();
        eeg.update(size, time_array_copy, data_array_copy);

        usleep(1000 * 16);
    }

    data_thread.join();

    /*
    SerialInterface sf("/dev/ttyUSB0");

    while (true) {
        printf("Line: %s\n", sf.getNextLine().c_str());
    }
    */

    return 0;
}
