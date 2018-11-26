#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"

#include "force-trainer.h"

void app_main(void)
{    
    nvs_flash_init();
    ESP_LOGI("status", "start");
    uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 17, 16, 18, 19));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, 1024 * 2, 0, 0, NULL, 0));
    ESP_LOGI("status", "initialized");

    handle_force_trainer();
}
