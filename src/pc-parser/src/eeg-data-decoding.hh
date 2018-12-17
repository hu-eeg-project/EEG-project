#pragma once

#include "rolling-array.hh"
#include "serial.hh"
#include "eeg-graph.hh"

void loopDecodeNonBatched(SerialInterface& sf,
                          ArrayPair<RollingArray<Double_t>,
                          RollingArray<Double_t>>* array,
                          std::atomic<bool>* close_thread);

void loopDecodeBatched(SerialInterface& sf,
                       ArrayPair<RollingArray<Double_t>,
                       RollingArray<Double_t>>* array,
                       const size_t frame_size,
                       std::atomic<bool>* close_thread);
