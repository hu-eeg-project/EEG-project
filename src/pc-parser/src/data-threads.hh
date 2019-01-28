/**
 * \file data-threads.hh
 * \brief Contains the different threads for data collection / generation
 */

#pragma once

#include <atomic>
#include <memory>

#include "rolling-array.hh"
#include "eeg-graph.hh"

/**
 * \brief Generates a wave with the given frequencies and amplitudes
 * \param array The Data and Time array to put the generated data in
 * \param sample_rate The rate at which data is sampled (generated in this case)
 * \param frequencies An array of frequencies with amplitudes to put in the signal
 * \param size The ammount of frequencies in the array
 * \param close_thread A flag to signal when the thread needs to close
 */
void testThread(ArrayPair<RollingArray<Double_t>,
                RollingArray<Double_t>>* array,
                uint32_t sample_rate,
                const Frequency_t* frequencies,
                size_t size,
                std::atomic<bool>* close_thread)
{
    Noise_t noise = {0, 60};
    WaveGenerator wave(frequencies, size, array);
    double t = 1.0f / sample_rate * 1000000;

    while(!*close_thread){
        array->lock();
        wave.genSample();
        array->unlock();

        usleep(t);
    }
}

/**
 * \brief Reads data from the serial port and puts it in the arrays
 * \param array The Data and Time array to put the read data in
 * \param close_thread A flag to signal when the thread needs to close
 */
void serialThread(ArrayPair<RollingArray<Double_t>,
                  RollingArray<Double_t>>* array,
                  std::atomic<bool>* close_thread)
{
    SerialConfig_t sConfig = {(std::string)"/dev/ttyUSB0", B115200};
    size_t frame_size = 16;
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    st = std::chrono::high_resolution_clock::now();

    SerialInterface sf(sConfig);

    printf("Waitig for batch specifier...\n");
    std::string input = sf.getNextLine();
    while(input[0] != 'b'){
        input = sf.getNextLine();
    }
    if(input[2] == '1'){
        printf("Decoding batched\n");
        loopDecodeBatched(sf, array, frame_size, close_thread);
    }else{
        printf("Decoding non batched\n");
        loopDecodeNonBatched(sf, array, close_thread);
    }
}
