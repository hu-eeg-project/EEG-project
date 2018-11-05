/*
 * \file eeg-graph.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "eeg-graph.hh"

#include <stdio.h>
#include <fftw3.h>
#include <iostream>

EEGGraph::EEGGraph(int* argc, char** argv) :
m_app("EEG Visualizer", argc, argv)
{
    //m_app = TApplication("EEG Visualizer", argc, argv);
    if (gROOT->IsBatch()) {
        printf("%s: cannot run in batch mode\n", argv[0]);
        return;
    }

    // -------------------------------------------------------------

    m_canvas = new TCanvas("c1","FFT Graph Canvas",0,0,1920,1080);
    m_pad1 = new TPad("c1_1", "c1_1", 0.01, 0.67, 0.99, 0.99);
    m_pad2 = new TPad("c1_2", "c1_2", 0.01, 0.01, 0.99, 0.66);


    m_pad1->SetGrid();
    m_pad2->SetGridx();

    m_pad1->Draw();
    m_pad2->Draw();

    const Int_t n = 1;
    Double_t x[n] = {0};
    Double_t y[n] = {0};

    m_pad1->cd();
    m_graph = new TGraph(n,x,y);
    m_graph->SetLineColor(2);
    m_graph->SetLineWidth(1);
    m_graph->SetMarkerColor(4);
    m_graph->SetMarkerStyle(1);
    m_graph->SetTitle("EEG Signal");
    m_graph->GetXaxis()->SetTitle("Time");
    m_graph->GetYaxis()->SetTitle("Amplitude");
    m_graph->Draw("APL");
    // TCanvas::Update() draws the frame, after which one can change it
    //m_canvas->Update();
    //m_canvas->GetFrame()->SetBorderSize(12);
    //m_canvas->Modified();

    m_pad2->cd();
    m_fft = new TH1F("fft", "eeg_fft", 3, 0, 3);
    m_fft->SetTitle("EEG FFT");
    m_fft->SetFillStyle(3001);
    m_fft->SetFillColor(30);
    m_fft->Draw("B HIST");
    m_amp_max = 100;
    m_fft->SetMinimum(0);
    m_fft->SetMaximum(m_amp_max);

    m_fft->SetStats(false);

    m_app.Show();
}

int counter = 0;

void EEGGraph::updateGraph(unsigned int points, Double_t* x, Double_t* y)
{
    m_pad1->cd();
    m_graph->Set(points);
    for (unsigned int i = 0; i < points; i++) {
        m_graph->SetPoint(i, x[i], y[i]);
    }
    m_graph->GetYaxis()->SetRangeUser(-300,300);
}

void EEGGraph::updateFFT(unsigned int frequencies, double* values)
{
    m_pad2->cd();
    m_fft->Reset();

    char buffer[32];

    for (int i = 0; i < frequencies; i++) {
        sprintf(buffer, "%i\0", i + 1);
        m_fft->Fill(buffer, values[i]);

        if (values[i] > m_amp_max) {
            m_amp_max = values[i];
            m_fft->SetMaximum(m_amp_max);
        }
    }

    m_pad2->Modified();
}

void EEGGraph::render()
{
    m_canvas->Modified();
    m_canvas->Update();
}

void EEGGraph::update(unsigned int points, Double_t* x, Double_t* y)
{
    int index = -1;
    for (int i = points - 1; i >= 0; i--) {
        if (x[points - 1] - x[i] >= 1) {
            index = i;
            break;
        }
    }
    if (index == -1){
	return;
    }

    Double_t * result_y = new Double_t[points];
    memcpy(result_y, y, points);
    
    size_t fft_size = points - index;
    double fft_result[512][2];
    double buffer[256] = {0};
    fftw_plan plan = fftw_plan_dft_r2c_1d(fft_size,
                                          y + (points - fft_size),
                                          fft_result,
                                          0);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    for(int i = 2;i<fft_size/2;i++){
	fft_result[i][0] = 0;
	fft_result[i][1] = 0;
    }

    plan = fftw_plan_dft_c2r_1d(fft_size, fft_result, result_y, 0);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    for(int i = 0;i<points;i++){
	result_y[i] *= 1.0f/points;
    }

    updateGraph(points, x, result_y);
    
    for (int i = 1; i < fft_size / 2 - 1; i++) {
        buffer[i - 1] = sqrt(pow(fft_result[i][0], 2) +
                             pow(fft_result[i][1], 2));
    }

    

    updateFFT(50, buffer);
    render();
}
