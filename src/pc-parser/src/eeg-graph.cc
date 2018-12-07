/*
 * \file eeg-graph.cc
 * \brief
 * \author kasumi
 * \copyright Copyright (c) 2018, kasumi
 * \license See LICENSE
 */
#include "eeg-graph.hh"

#include <fftw3.h>
#include <iostream>

EEGGraph::EEGGraph(int* argc, char** argv, const int ay_min, const int ay_max) :
    m_app("EEG Visualizer", argc, argv),
    ay_min(ay_min),
    ay_max(ay_max)
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

    st = std::chrono::high_resolution_clock::now();
}

int counter = 0;

void EEGGraph::updateGraph(unsigned int points, Double_t* x, Double_t* y)
{
    m_pad1->cd();
    m_graph->Set(points);
    for (unsigned int i = 0; i < points; i++) {
        m_graph->SetPoint(i, x[i], y[i]);
    }
    m_graph->GetYaxis()->SetRangeUser(ay_min,ay_max);
}

void EEGGraph::updateFFT(unsigned int frequencies, double* values)
{
    if (frequencies > 50) frequencies = 50;

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

#define POINTS 128

double TimePoints[] = {0.0078125,
                       0.015625,
                       0.03125,
                       0.0625,
                       0.125,
                       0.25,
                       0.5,
                       1};

int EEGGraph::findTimePoint(const double timepoint,
                  const unsigned int points,
                  const Double_t* x)
{
    int index = -1;
    for (int i = points - 1; i >= 0; i--) {
        if (x[points - 1] - x[i] >= timepoint) {
            index = i;
            break;
        }
    }
    return index;
}

struct EEGGraph::timeIndexPair {
    double timepoint;
    int index;
};

EEGGraph::timeIndexPair EEGGraph::findBiggestTimePoint(const unsigned int points, const Double_t* x)
{
    timeIndexPair ret = {0, -1};

    for (int i = 0; i < sizeof(TimePoints) / sizeof(TimePoints[0]); i++) {
        int val = findTimePoint(TimePoints[i], points, x);
        if (val != -1) ret = {TimePoints[i], val};
    }

    return ret;
}

void EEGGraph::update(unsigned int points, Double_t* x, Double_t* y)
{
    if (!points) return;

    updateGraph(points, x, y);

    timeIndexPair timepoint = findBiggestTimePoint(points, x);

    const int num_points_frame = points - timepoint.index;
    const int bin_size = (int) (1.0 / timepoint.timepoint);
    const int max_frequency = num_points_frame / bin_size / 2;

    /*
    printf("TimePoints: %f @ %3i of %3i, bins: %i max freq: %i Hz\n",
           timepoint.timepoint,
           timepoint.index,
           num_points_frame,
           bin_size,
           max_frequency);
    */

    if ((points - timepoint.index) /
        ((int) (1.0 / timepoint.timepoint)) / 2
        < 1) return;

    size_t fft_size = points - timepoint.index;
    double fft_result[2048][2];
    double buffer[1024] = {0};

    fftw_plan plan = fftw_plan_dft_r2c_1d(fft_size,
                                          y + (points - fft_size),
                                          fft_result,
                                          0);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    for (int i = 1; i < (fft_size - 2) / 2; i++) {
        buffer[i - 1] = sqrt(pow(fft_result[i][0], 2) +
                             pow(fft_result[i][1], 2));
    }

    /*
    printf("Time: %f - %f = %f\n",
           x[points - 1],
           x[points - fft_size - 1],
           x[points - 1] - x[points - fft_size - 1]);
    */

    updateFFT(num_points_frame / bin_size / 2, buffer);
    render();
}

void EEGGraph::filter_freq(const size_t points,
                           const double* time_src,
                           const double* wave_src,
                           RollingArray<Double_t>* wave_dest,
                           RollingArray<Double_t>* time_dest,
                           FrequencyFilter_t filter)
{
    if (!points) return;
/*
    std::chrono::time_point<std::chrono::high_resolution_clock> nt;
    nt = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = nt - filter_last_time;
    if(dt.count() < 1.f){
        return;
    }
    filter_last_time = nt;
*/
    timeIndexPair timepoint = findBiggestTimePoint(points, time_src);

    if ((points - timepoint.index) /
        ((int) (1.0 / timepoint.timepoint)) / 2
        < 1) return;

    Double_t * result_y = new Double_t[points];
    for(int i = 0;i<points;i++){
        result_y[i] = wave_src[i];
    }

    size_t fft_size = points - timepoint.index;
    fftw_complex fft_result[2048];
    double filtered_result[2048];
    
    fftw_plan plan = fftw_plan_dft_r2c_1d(fft_size,
                                          result_y + (points - fft_size),
                                          fft_result,
                                          0);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    delete[] result_y;

    for(int i = 0;i<filter.len;i++){
        fft_result[filter.frequencies[i]][0] = 0;
        fft_result[filter.frequencies[i]][1] = 0;
    }
    if(filter.hp_cutoff){
        for(int i = filter.hp_cutoff;i>=0;i--){
            fft_result[i][0] = 0;
            fft_result[i][1] = 0;
        }
    }
    if(filter.lp_cutoff){
        for(int i = filter.lp_cutoff;i<fft_size;i++){
            fft_result[i][0] = 0;
            fft_result[i][1] = 0;
        }
    }
  
    fftw_plan fft_inv_plan = fftw_plan_dft_c2r_1d(fft_size, fft_result, filtered_result, 0);
    fftw_execute(fft_inv_plan);
    fftw_destroy_plan(fft_inv_plan);

    int i = 0;
    for(i; i < fft_size; i++){
        if(time_src[timepoint.index+i] > last_time){
            wave_dest->append(filtered_result[i] * 1.0f/fft_size);
            time_dest->append(time_src[timepoint.index+i]);
        }
    } 
    last_time = time_src[timepoint.index+i-1];
}
