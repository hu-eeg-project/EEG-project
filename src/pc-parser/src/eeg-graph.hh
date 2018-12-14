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

typedef struct{
    int* frequencies;
    size_t len;
    int lp_cutoff;
    int hp_cutoff;
}FrequencyFilter_t;

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

    std::chrono::time_point<std::chrono::high_resolution_clock> st, nt;
    double last_time = 0;

    struct timeIndexPair;

    void updateGraph(unsigned int points, Double_t* x, Double_t* y);
    void updateFFT(unsigned int frequencies, double* values);
    void render();
    timeIndexPair findBiggestTimePoint(const unsigned int points, const Double_t* x);

public:
    EEGGraph(int* argc,
             char** argv,
             const int ay_min,
             const int ay_max);

    void update(unsigned int points, Double_t* x, Double_t* y);

    void filter_freq(const size_t points,
                               const double* time_src,
                               const double* wave_src,
                               RollingArray<Double_t>* wave_dest,
                               RollingArray<Double_t>* time_dest,
                               FrequencyFilter_t filter);

    int findTimePoint(const double timepoint,
                      const unsigned int points,
                      const Double_t* x);
};
