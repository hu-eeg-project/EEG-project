#include "force-trainer.h"

#include <stdio.h>
#include <stdarg.h>

#include "driver/uart.h"
#include "esp_timer.h"

void read_bytes_from_ft(uint8_t* data, const size_t len){
    uart_read_bytes(UART_NUM_2, data, len, portMAX_DELAY);
}

void write_bytes_pc(const char* format, ...){
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int64_t timer_get_us(){
    return esp_timer_get_time();
}
