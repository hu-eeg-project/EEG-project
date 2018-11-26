#pragma once
#include <stdint.h>
#include <stddef.h>

void handle_force_trainer();

// Functions that has to be ported
void read_bytes_from_ft(uint8_t* data, const size_t len);
void write_bytes_pc(const char* format, ...);
int64_t timer_get_us();


