#pragma once

#include <chrono>

#include "rolling-array.hh"
#include <random>

typedef struct {
    double amp;
    uint16_t freq;
}Frequency_t;

typedef struct{
    double mean;
    double stddef;
}Noise_t;

class WaveGenerator{
    const Frequency_t * frequencies;
    size_t size;
    ArrayPair<RollingArray<double>, RollingArray<double>> * aPair;

    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;

    std::default_random_engine generator;
    std::normal_distribution<double> dist;
    
public:
    WaveGenerator(const Frequency_t * frequencies,
		  size_t size,
		  ArrayPair<RollingArray<double>, RollingArray<double>> * aPair);

    WaveGenerator(const Frequency_t * frequencies,
		  size_t size,
		  ArrayPair<RollingArray<double>, RollingArray<double>> * aPair,
		  const Noise_t noise);
    
    void genSample();

    bool noise;
};
