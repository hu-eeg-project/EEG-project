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
#include <termios.h>
#include <chrono>
#include <iostream>
#include <sstream>

#define NUMBER_OF_POINTS 1000
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

void serialThread(ArrayPair<RollingArray<int16_t>,
		  RollingArray<Double_t>>* array, const SerialConfig_t sConfig, size_t frame_size)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    st = std::chrono::high_resolution_clock::now();

    size_t frame_index = 0;
    int16_t * frame = new int16_t[frame_size];
    int32_t total_dur = 0;
    
    SerialInterface sf(sConfig);
    while (true) {
	//auto test1 = std::chrono::high_resolution_clock::now();
	std::string input = sf.getNextLine();
	char type, delim;
	int16_t eeg_data, frame_dur;
	if(input.size()){
	    std::stringstream stream(input);
	    stream >> type >> delim >> eeg_data >> delim >> frame_dur;
	    if(type == 'd'){
		frame[frame_index] = eeg_data;
		frame_index+=1;
		total_dur += frame_dur;
		if(frame_index >= frame_size){
		    nt = std::chrono::high_resolution_clock::now();
		    std::chrono::duration<double> dt = nt-st;
		    double end_time = dt.count() + total_dur;
		    double avrg_frame_time = (double)total_dur / frame_size / 1000000;
		    
		    array->lock();
		    for(int i = 0;i<frame_index;i++){
			array->array1.append(frame[i]);
			array->array2.append(dt.count() + avrg_frame_time * i);
		    }
		    array->unlock();
		    
		    frame_index = 0;
		    total_dur = 0;
		}
//std::chrono::duration<double> dt = nt-st;
		//array->lock();
		//array->array1.append(eeg_data);
		//array->array2.append(dt.count());
		//array->unlock();
	    }
	}
    }
}

int main(int argc, char* argv[])
{

    EEGGraph eeg(&argc, argv);

    RollingArray<Double_t> data_array(NUMBER_OF_POINTS);
    RollingArray<Double_t> time_array(NUMBER_OF_POINTS);
    ArrayPair<RollingArray<Double_t>, RollingArray<Double_t>> data(data_array, time_array);
    //SerialConfig_t sConfig = {(std::string)"/dev/ttyUSB0", B115200};
    //std::thread data_thread(serialThread, &data, sConfig, 16);
    std::thread data_thread(testThread, &data, NUMBER_OF_POINTS / FRAME_DURATION);

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
