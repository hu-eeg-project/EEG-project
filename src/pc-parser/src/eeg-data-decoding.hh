#pragma once

#include "rolling-array.hh"
#include "serial.hh"
#include "eeg-graph.hh"

void loopDecodeNonBatched(SerialInterface& sf,
                          ArrayPair<RollingArray<int16_t>, RollingArray<Double_t>>* array);

void loopDecodeBatched(SerialInterface& sf,
                       ArrayPair<RollingArray<int16_t>, RollingArray<Double_t>>* array,
                       const size_t frame_size);
