#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/adc.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

    printf("b:0\n");

    while (true) {

        int val = adc1_get_raw(ADC1_CHANNEL_0);

        printf("d:%i\n", val);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
