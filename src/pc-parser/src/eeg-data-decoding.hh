#pragma once

#include "rolling-array.hh"
#include "serial.hh"
#include "eeg-graph.hh"

/**
 * \brief Decodes data from the serial port that is not batched
 * \param sf The serial interface to read from
 * \param array The data array to put the data and timestamps into
 * \param close_thread A flag to signal when the thread needs to close
 */
void loopDecodeNonBatched(SerialInterface& sf,
                          ArrayPair<RollingArray<Double_t>,
                          RollingArray<Double_t>>* array,
                          std::atomic<bool>* close_thread);

/**
 * \brief Decodes data from the serial port that is batched
 * \param sf The serial interface to read from
 * \param array The data array to put the data and timestamps into
 * \param frame_size The size of a batched frame
 * \param close_thread A flag to signal when the thread needs to close
 */
void loopDecodeBatched(SerialInterface& sf,
                       ArrayPair<RollingArray<Double_t>,
                       RollingArray<Double_t>>* array,
                       const size_t frame_size,
                       std::atomic<bool>* close_thread);
