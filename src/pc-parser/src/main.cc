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
#include "data-threads.hh"

#include <thread>
#include <math.h>
#include <time.h>
#include <termios.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define NUMBER_OF_POINTS 1000
#define FRAME_DURATION 3

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


int main(int argc, char* argv[])
{
    gengetopt_args_info config;
    if (cmdline_parser(argc, argv, &config) != 0) {
        exit(1);
    }

    FrequencyFilter_t filter = {0};
    Frequency_t* frequencies = NULL;

    Display* display = NULL;

    bool filtering = false;

    int ay_min, ay_max;

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

        if(config.filter_frequency_given){
            filtering = true;
            for(int i = 0;i<config.filter_frequency_given;i++){
                filter.frequencies = config.filter_frequency_arg;
            }
            filter.len = config.filter_frequency_given;
        }
        if(config.hp_cutoff_given){
            filtering = true;
            filter.hp_cutoff = config.hp_cutoff_arg;
        }
        if(config.lp_cutoff_given){
            filtering = true;
            filter.lp_cutoff = config.lp_cutoff_arg;
        }

    } else {
        printf("No data source!\n");
        exit(1);
    }

    if(config.ay_range_given == 2){
        ay_min = config.ay_range_arg[0];
        ay_max = config.ay_range_arg[1];
    }else{
        ay_min = -1000;
        ay_max = 1000;
    }

    EEGGraph eeg(&argc, argv, ay_min, ay_max);
    RollingArray<Double_t> data_array(NUMBER_OF_POINTS);
    RollingArray<Double_t> time_array(NUMBER_OF_POINTS);
    ArrayPair<RollingArray<Double_t>, RollingArray<Double_t>> data(data_array, time_array);

    std::string start_time_string = getTimeAndDate();

    if (config.p300_flag) {
        if (config.verbose_flag) printf("Creating P300 display\n");
        display = new Display();

        DIR* d = opendir(start_time_string.c_str());
        if (d) closedir(d);
        else {
            int r = mkdir(start_time_string.c_str(),
                          S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if (r == -1) {
                printf("Failed to create directory for logs!\n");
                exit(-1);
            }
        }
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

    RollingArray<Double_t> filtered_data(NUMBER_OF_POINTS);
    RollingArray<Double_t> filtered_time(NUMBER_OF_POINTS);

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
                    filename = start_time_string + "/"
                             + getTimeAndDate() + ".eeg";

                    file.open(filename);
                    if (!file.is_open()) {
                        printf("Failed to create file!\n");
                    }

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
        if(filtering){
            eeg.filter_freq(size,
                            time_array_copy,
                            data_array_copy,
                            &filtered_data,
                            &filtered_time,
                            filter);
            eeg.update(size, filtered_time.getData(), filtered_data.getData());
        }else{
            eeg.update(size, time_array_copy, data_array_copy);
        }
        usleep(1000 * 16);
    }

    data_thread.join();

    if (frequencies) delete[] frequencies;
    if (display) delete display;

    return 0;
}
