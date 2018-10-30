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

#define NUMBER_OF_POINTS 500
#define FRAME_DURATION 2

void dataThread(ArrayPair<RollingArray<Double_t>,
                RollingArray<Double_t>>* array, uint32_t sample_rate)
{
    Frequency_t frequencies[] = {{500, 1}, {100, 10}};
    Noise_t noise = {0, 10};
    WaveGenerator wave(frequencies, sizeof(frequencies)/sizeof(Frequency_t), array, noise);
    double t = 1.0f / sample_rate * 1000000;
    
    while (true) {
        array->lock();
        wave.genSample();
        array->unlock();

        usleep(t);
    }
}

int main(int argc, char* argv[])
{
    EEGGraph eeg(&argc, argv);
    
    RollingArray<Double_t> data_array(NUMBER_OF_POINTS);
    RollingArray<Double_t> time_array(NUMBER_OF_POINTS);
    ArrayPair<RollingArray<Double_t>, RollingArray<Double_t>> data(data_array, time_array);

    std::thread data_thread(dataThread, &data, NUMBER_OF_POINTS / FRAME_DURATION);

    Double_t data_array_copy[NUMBER_OF_POINTS];
    Double_t time_array_copy[NUMBER_OF_POINTS];

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
