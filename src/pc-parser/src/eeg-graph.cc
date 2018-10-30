/*
 * \file eeg-graph.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "eeg-graph.hh"

#include <stdio.h>

EEGGraph::EEGGraph(int* argc, char** argv) :
m_app("EEG Visualizer", argc, argv)
{
    //m_app = TApplication("EEG Visualizer", argc, argv);
    if (gROOT->IsBatch()) {
        printf("%s: cannot run in batch mode\n", argv[0]);
        return;
    }

    // -------------------------------------------------------------

    m_canvas = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
    m_canvas->SetGrid();
    const Int_t n = 20;
    Double_t x[n], y[n];
    for (Int_t i=0;i<n;i++) {
        x[i] = i*0.1;
        y[i] = 10*sin(x[i]+0.2);
        printf(" i %i %f %f \n",i,x[i],y[i]);
    }
    m_graph = new TGraph(n,x,y);
    m_graph->SetLineColor(2);
    m_graph->SetLineWidth(4);
    m_graph->SetMarkerColor(4);
    m_graph->SetMarkerStyle(2);
    m_graph->SetTitle("a simple graph");
    m_graph->GetXaxis()->SetTitle("X title");
    m_graph->GetYaxis()->SetTitle("Y title");
    m_graph->GetYaxis()->SetRangeUser(-1000,1000);
    m_graph->Draw("ACP");
    // TCanvas::Update() draws the frame, after which one can change it
    m_canvas->Update();
    m_canvas->GetFrame()->SetBorderSize(12);
    m_canvas->Modified();

    m_app.Show();
}

int counter = 0;

void EEGGraph::updateGraph(unsigned int points, Double_t* x, Double_t* y)
{
    m_graph->Set(points);
    for (unsigned int i = 0; i < points; i++) {
        m_graph->SetPoint(i, x[i], y[i]);
    }
    m_graph->GetYaxis()->SetRangeUser(-1000,1000);
}

void EEGGraph::render()
{
    m_canvas->Update();
}

void EEGGraph::update(unsigned int points, Double_t* x, Double_t* y)
{
    updateGraph(points, x, y);
    render();
}
