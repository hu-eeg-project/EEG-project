#pragma once

#include <chrono>

#include "rolling-array.hh"

typedef struct {
    double amp;
    uint16_t freq;
}Frequency_t;

class WaveGenerator{
    const Frequency_t * frequencies;
    size_t size;
    ArrayPair<RollingArray<double>, RollingArray<double>> * aPair;
    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;

public:
    WaveGenerator(const Frequency_t * frequencies,
		  size_t size,
		  ArrayPair<RollingArray<double>, RollingArray<double>> * aPair);
    void genSample();
};
