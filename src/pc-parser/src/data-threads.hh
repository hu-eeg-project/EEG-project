#pragma once

#include "rolling-array.hh"
#include "eeg-graph.hh"

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
