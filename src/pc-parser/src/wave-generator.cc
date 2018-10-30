#include "wave-generator.hh"

#include <math.h>
#include <iostream>

WaveGenerator::WaveGenerator(const Frequency_t * frequencies,
			     size_t size,
			     ArrayPair<RollingArray<double>, RollingArray<double>> * aPair):
    frequencies(frequencies),
    size(size),
    aPair(aPair)
{
    st = std::chrono::high_resolution_clock::now();
}

void WaveGenerator::genSample(){
    double sample = 0;
    nt = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = nt-st;
    for(int i = 0;i<size;i++){
	sample += frequencies[i].amp * sin(frequencies[i].freq * 2.0 * M_PI * dt.count());
    }
    //std::cout << sample << ": " << dt.count() << std::endl;
    aPair->array1.append(sample);
    aPair->array2.append(dt.count());
}
