#pragma once
#include <stdint.h>
#include <stddef.h>

// The funtion that receives and parses all the data from the Froce Trainer
void handle_force_trainer();

// Functions that have to be ported:
// This function should read an amount of bytes specified bij len. Should also be blocking.
void read_bytes_from_ft(uint8_t* data, const size_t len);

// Function for sending bytes to the PC. Works the same as printf.
void write_bytes_pc(const char* format, ...);

// Function for getting the amount of microseconds since start of program.
int64_t timer_get_us();


