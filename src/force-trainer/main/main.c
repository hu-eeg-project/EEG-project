#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"

#include "force-trainer.h"

#define TX_PIN  17
#define RX_PIN  16
#define RTS_PIN 18
#define CTS_PIN 19

void app_main(void)
{    
    nvs_flash_init();
    ESP_LOGI("status", "start");

    // Init the UART interface for sending data to pc.
    uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    // Init UART_NUM_2 for recieving data from force_trainer.
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, TX_PIN, RX_PIN, RTS_PIN, CTS_PIN));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, 1024 * 2, 0, 0, NULL, 0));
    ESP_LOGI("status", "initialized");

    // The function for recieving and parsing data from Force Trainer
    handle_force_trainer();
}
