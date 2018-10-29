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
#include <TGraph.h>
#include <TApplication.h>
#include <TROOT.h>

class EEGGraph
{
    TApplication m_app;
    TCanvas* m_canvas;
    TGraph* m_graph;

    void updateGraph();
    void render();

public:
    EEGGraph(int* argc, char** argv);

    void update();
};
