/*
 * \file eeg-graph.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <math.h>
#include <chrono>

#include <TAxis.h>
#include <TFrame.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TROOT.h>

#include "rolling-array.hh"

/**
 * A structure containing data what to filter
 */
typedef struct{
    int* frequencies; /*!< Specific frequencies to remove */
    size_t len; /*!< Amount of frequencies passed in the array */
    int lp_cutoff; /*!< Lowpass filter cutoff frequency */
    int hp_cutoff; /*!< Highpass filter cutoff frequency */
}FrequencyFilter_t;

/**
 * \brief The graph rendering the data through ROOT
 */
class EEGGraph
{
    TApplication m_app;
    TCanvas* m_canvas;
    TPad* m_pad1;
    TPad* m_pad2;
    TGraph* m_graph;
    TH1F* m_fft;
    double m_amp_max;

    int ay_min, ay_max;

    bool m_display_fft;

    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    double last_time = 0;

    struct timeIndexPair;

    void updateGraph(unsigned int points, Double_t* x, Double_t* y);
    void updateFFT(unsigned int frequencies, double* values);
    void render();
    timeIndexPair findBiggestTimePoint(const unsigned int points, const Double_t* x);

public:
    /**
     * \brief Creates an EEG Graph window
     * \param argc The amount of arguments the program has
     * \param argv The arguments the program has
     * \param ay_min The minimum value on the y axis of the graph
     * \param ay_max The maximum value on the y axis of the graph
     * \param display_fft Wether or not to draw an FFT diagram of the incomming signal
     */
    EEGGraph(int* argc,
             char** argv,
             const int ay_min,
             const int ay_max,
             bool display_fft);

    /**
     * \brief Updates the display with the new data
     * \param points The ammount of data points
     * \param x The x values of the data points
     * \param y The y values of the data points
     */
    void update(unsigned int points, Double_t* x, Double_t* y);

    /**
     * \brief Filters frequencies
     * \param points Amount of points in the array
     * \param time_src The time points from the input
     * \param wave_src The data points from the input
     * \param wave_dest The array where the filtered data is output
     * \param time_dest The array where the filtered time is output
     * \param filter The filter settings
     */
    void filter_freq(const size_t points,
                     const double* time_src,
                     const double* wave_src,
                     RollingArray<Double_t>* wave_dest,
                     RollingArray<Double_t>* time_dest,
                     FrequencyFilter_t filter);

    /**
     * \brief Finds if a given timepoint exists
     * \param timepoint The timepoint to search
     * \param points The ammount of points in the array
     * \param x The time array
     * \return Returns -1 if the given timepoint is not found, otherwise the index in the array of the given timepoint
     */
    int findTimePoint(const double timepoint,
                      const unsigned int points,
                      const Double_t* x);
};
