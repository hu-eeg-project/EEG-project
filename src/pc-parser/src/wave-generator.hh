#pragma once

#include <chrono>

#include "rolling-array.hh"
#include <random>

/**
 * \struct Frequency_t
 * \brief A structure to represent a frequency in a signal.
 */
typedef struct{
    double amp; /*!< The amplitude of this frequency. */
    uint16_t freq; /*!< The frequency. */
}Frequency_t;

/**
 * @struct Noise_t
 * \brief A structure to represent noise in a signal.
 */
typedef struct{
    double mean; /*!< The average of the noise. */
    double stddef; /*!< The standard deviation of the noise. */
}Noise_t;

/**
 * @class WaveGenerator
 * \brief A class for generating signals consisting of given frequencies. Noise can also be added to the signal.
 */
class WaveGenerator{
    const Frequency_t * frequencies;
    size_t size;
    ArrayPair<RollingArray<double>, RollingArray<double>> * aPair;

    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;

    std::default_random_engine generator;
    std::normal_distribution<double> dist;
    
public:
    /** 
     * \brief The contructor for generating a wavegenerator without noise.
     *
     * \param frequencies An array of frequencies the signal should consist of.
     * \param size The size of the frequency array.
     * \param aPair An ArrayPair the signal should be stored in.
     */
    WaveGenerator(const Frequency_t * frequencies,
		  size_t size,
		  ArrayPair<RollingArray<double>, RollingArray<double>> * aPair);

    /** 
     * \brief The contructor for generating a wavegenerator with noise.
     *
     * \param frequencies An array of frequencies the signal should consist of.
     * \param size The size of the frequency array.
     * \param aPair An ArrayPair the signal should be stored in.
     * \param noise Characteristisc of the noise.
     */
    WaveGenerator(const Frequency_t * frequencies,
		  size_t size,
		  ArrayPair<RollingArray<double>, RollingArray<double>> * aPair,
		  const Noise_t noise);
    /** 
     * \brief Everytime this function is called a sample is generated and put into aPair.
     * \see WaveGenerator()
     */
    void genSample();

    /**
     * \brief Flag for noise or no noise.
     */
    bool noise;
};
