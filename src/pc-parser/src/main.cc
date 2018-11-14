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
#include "eeg-data-decoding.hh"

#include <thread>
#include <math.h>
#include <time.h>
#include <termios.h>
#include <chrono>
#include <iostream>
#include <sstream>

#define NUMBER_OF_POINTS 300
#define FRAME_DURATION 3

void testThread(ArrayPair<RollingArray<Double_t>,
                RollingArray<Double_t>>* array, uint32_t sample_rate)
{
    Frequency_t frequencies[] = {{200, 1}, {20, 10}};
    Noise_t noise = {0, 60};
    WaveGenerator wave(frequencies, sizeof(frequencies)/sizeof(Frequency_t), array, noise);
    double t = 1.0f / sample_rate * 1000000;

    while (true) {
        array->lock();
        wave.genSample();
        array->unlock();

        usleep(t);
    }
}

void serialThread(ArrayPair<RollingArray<uint16_t>,
                  RollingArray<Double_t>>* array)
{
    SerialConfig_t sConfig = {(std::string)"/dev/ttyUSB0", B115200};
    size_t frame_size = 16;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    st = std::chrono::high_resolution_clock::now();

    SerialInterface sf(sConfig);

    std::string input = sf.getNextLine();
    printf("Waitig for batch specifier...\n");
    while(true){
        if(input[0] == 'b'){
            if(input[0] == '1'){
                printf("Decoding batched\n");
                loopDecodeBatched(sf, array, frame_size);
            }else{
                printf("Decoding non batched\n");
                loopDecodeNonBatched(sf, array);
            }
            break;
        }
        input = sf.getNextLine();
    }
}

int main(int argc, char* argv[])
{

    EEGGraph eeg(&argc, argv);
    printf("hello\n");
    RollingArray<uint16_t> data_array(NUMBER_OF_POINTS);
    RollingArray<Double_t> time_array(NUMBER_OF_POINTS);
    ArrayPair<RollingArray<uint16_t>, RollingArray<Double_t>> data(data_array, time_array);
    SerialConfig_t sConfig = {(std::string)"/dev/ttyUSB0", B115200};
    std::thread data_thread(serialThread, &data);
    //std::thread data_thread(testThread, &data, NUMBER_OF_POINTS / FRAME_DURATION);
    
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
    return 0;
}
