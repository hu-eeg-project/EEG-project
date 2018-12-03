/*
 * \file main.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */

#include <stdio.h>
#include <unistd.h>

#include "cmdline.h"

#include "serial.hh"
#include "eeg-graph.hh"
#include "rolling-array.hh"
#include "wave-generator.hh"
#include "eeg-data-decoding.hh"
#include "display.hh"

#include <thread>
#include <math.h>
#include <time.h>
#include <termios.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>

#define NUMBER_OF_POINTS 1000
#define FRAME_DURATION 3

void testThread(ArrayPair<RollingArray<Double_t>,
                RollingArray<Double_t>>* array,
                uint32_t sample_rate,
                const Frequency_t* frequencies,
                size_t size)
{
    Noise_t noise = {0, 60};
    WaveGenerator wave(frequencies, size, array);
    double t = 1.0f / sample_rate * 1000000;

    while (true) {
        array->lock();
        wave.genSample();
        array->unlock();

        usleep(t);
    }
}

std::string getTimeAndDate()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[128];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", timeinfo);
    return std::string(buffer);
}

void serialThread(ArrayPair<RollingArray<Double_t>,
                  RollingArray<Double_t>>* array)
{
    SerialConfig_t sConfig = {(std::string)"/dev/ttyUSB0", B115200};
    size_t frame_size = 16;
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    st = std::chrono::high_resolution_clock::now();

    SerialInterface sf(sConfig);

    printf("Waitig for batch specifier...\n");
    std::string input = sf.getNextLine();
    while(true){
        if(input[0] == 'b'){
            if(input[2] == '1'){
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
    gengetopt_args_info config;
    if (cmdline_parser(argc, argv, &config) != 0) {
        exit(1);
    }

    Frequency_t* frequencies = NULL;
    Display* display = NULL;

    if (config.serial_flag) {
        if (config.verbose_flag) printf("Using Serial as data source.\n");
    } else if (config.generator_flag) {
        if (config.verbose_flag) printf("Using Generator as data source.\n");
        if (config.frequency_given != config.amplitude_given) {
            printf("Same amount of frequencies and amplitudes needed!\n");
            exit(1);
        }

        frequencies = new Frequency_t[config.frequency_given];
        if (!frequencies) {
            printf("Failed to allocate memory for frequencies.\n");
            exit(1);
        }

        for (int i = 0; i < config.frequency_given; i++) {
            frequencies[i] = {config.amplitude_arg[i], (uint16_t) config.frequency_arg[i]};
        }
    } else {
        printf("No data source!\n");
        exit(1);
    }

    EEGGraph eeg(&argc, argv);
    RollingArray<Double_t> data_array(NUMBER_OF_POINTS);
    RollingArray<Double_t> time_array(NUMBER_OF_POINTS);
    ArrayPair<RollingArray<Double_t>, RollingArray<Double_t>> data(data_array, time_array);

    if (config.p300_flag) {
        if (config.verbose_flag) printf("Creating P300 display\n");
        display = new Display();
    }

    std::thread data_thread;
    if (config.serial_flag) {
        SerialConfig_t sConfig = {std::string(config.interface_arg), B115200};
        data_thread = std::thread(serialThread, &data);
    } else {
        data_thread = std::thread(testThread, &data, NUMBER_OF_POINTS / FRAME_DURATION, frequencies, config.frequency_given);
    }

    Double_t data_array_copy[NUMBER_OF_POINTS];
    Double_t time_array_copy[NUMBER_OF_POINTS];

    std::ofstream file;
    std::string filename;
    double last_time;

    bool recording = false;
    bool looping = true;
    while (looping) {
        data.lock();
        int size = data.array1.getSize();
        for (int i = 0; i < size; i++) {
            data_array_copy[i] = data.array1[i];
            time_array_copy[i] = data.array2[i];
        }
        data.unlock();

        if (display) {
            display->update();
            if (display->recording()) {
                if (!recording) {
                    recording = true;
                    filename = getTimeAndDate() + ".📈";
                    file.open(filename);
                    last_time = time_array_copy[size - 1];
                }

                for (int i = 0; i < size; i++) {
                    if (time_array_copy[i] > last_time) {
                        last_time = time_array_copy[i];
                        file << time_array_copy[i] << ","
                             << data_array_copy[i] << std::endl;
                    }
                }

            } else {
                if (recording) {
                    recording = false;
                    file.close();
                    if (config.verbose_flag)
                        printf("Wrote P300 file [%s].\n", filename.c_str());
                }
            }
        }

        eeg.update(size, time_array_copy, data_array_copy);

        usleep(1000 * 16);
    }

    data_thread.join();

    if (frequencies) delete[] frequencies;
    if (display) delete display;

    return 0;
}
