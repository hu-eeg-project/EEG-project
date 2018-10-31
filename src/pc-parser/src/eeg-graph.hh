/*
 * \file eeg-graph.hh
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#pragma once

#include <math.h>
#include <TAxis.h>
#include <TFrame.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TROOT.h>

class EEGGraph
{
    TApplication m_app;
    TCanvas* m_canvas;
    TPad* m_pad1;
    TPad* m_pad2;
    TGraph* m_graph;
    TH1F* m_fft;
    unsigned int m_amp_max;

    void updateGraph(unsigned int points, Double_t* x, Double_t* y);
    void updateFFT(unsigned int frequencies, unsigned int* values);
    void render();

public:
    EEGGraph(int* argc, char** argv);

    void update(unsigned int points, Double_t* x, Double_t* y);
};
