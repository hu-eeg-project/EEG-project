#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_timer.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "string.h"

#define ERROR_UNKOWN_CMD    0x01
#define ERROR_BAD_SIGNAL    0x02
#define ERROR_BAD_CHECKSUM  0x03
#define ERROR_PACKET_LEN    0x04
#define ERROR_CREATING_TASK 0x05

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

    uint8_t data = 0;
    uint8_t pdata[170];
    uint8_t plength = 0;
    uint8_t pchecksum = 0;
    uint32_t packet_count = 0;

    char buff[1024];
    setvbuf(stdout, buff, _IOLBF, 1024);

    int64_t st = esp_timer_get_time();
    int64_t batch_timer = st;

    int16_t frame_buffer[20] = {0};
    size_t frame_index = 0;
    uint64_t frame_dur = 0;
    while(true){
        if((esp_timer_get_time()-batch_timer)>1000000){ // every second
            printf("b:1\n");
            batch_timer = esp_timer_get_time();
        }
        
        uart_read_bytes(UART_NUM_2, &data, 1, portMAX_DELAY);
        if (data != 0xAA) continue; // No Sync Packet
        uart_read_bytes(UART_NUM_2, &data, 1, portMAX_DELAY);
        if (data != 0xAA) continue; // No second Sync Byte
        ESP_LOGI("\nParsing Packet nr", "%d", packet_count++);

        uart_read_bytes(UART_NUM_2, &plength, 1, portMAX_DELAY);
        uint32_t checksum = 0;
        if(plength > 170){
            printf("r %d\n", ERROR_PACKET_LEN);
            ESP_LOGE("PACKET_LEN > 170", "plength: %d", plength);
            continue;
        }
        uart_read_bytes(UART_NUM_2, pdata, plength, portMAX_DELAY);
        uart_read_bytes(UART_NUM_2, &pchecksum, 1, portMAX_DELAY);
        for (int j = 0; j < plength; j++) {
            checksum += pdata[j];
            checksum &= 0xFF;
        }
        checksum = ~checksum & 0xFF;
        if (checksum != pchecksum) {
            ESP_LOGE("BAD_CHECKSUM", "%d != %d", checksum, pchecksum);
        }
        for (int j = 0; j < plength; j++) {
            switch (pdata[j])
            {
            case 0x01:
                ESP_LOGI("BATTERY:" , "%d", pdata[++j]);
                continue;               
            case 0x02: // Signal status
                printf("r:%d:%d\n", ERROR_BAD_SIGNAL, pdata[j + 1]);
                if(pdata[j + 1] > 0x10){
                    ESP_LOGE("POOR_SIGNAL", "%2x\n", pdata[++j]);
                }
                continue;
            case 0x03: // Hearthrate
                ESP_LOGI("HEART_RATE", "%2x\n", pdata[j + 1]);
                printf("h:%d\n", pdata[j+1]);
                j++;
                continue;
            case 0x04: //Attention
                ESP_LOGI("ATTENTION", "%2x\n", pdata[j + 1]);
                printf("a:%d\n", pdata[j+1]);
                j++;
                continue;
            case 0x05: // Meditation
                ESP_LOGI("MEDITATION", "%2x\n", pdata[j + 1]);
                printf("m:%d\n", pdata[j+1]);
                j++;
                continue;
            case 0x06: //An 8-bit Raw data of EEG data
                ESP_LOGI("8BIT_RAW", "%2x\n", pdata[j + 1]);
                j++;
                continue;
            case 0x07:
                ESP_LOGI("RAW_MARKER", "%2x\n", pdata[j + 1]);
                j++;
                continue;
            case 0x80: ; // C++ wants a statement after a label, so here is a semi-colon for an empty statement
                // Receive Raw EEG data from Neurosky chip on headset
                uint8_t rlength = pdata[j + 1];
                int16_t value = 0;
                for (int u = 0; u < rlength; u++) {
                    value = value << 8 | pdata[j + 2 + u];
                }
                ESP_LOGI("RAW_WAVE_VALUE", "%d", value);
                uint64_t nt = esp_timer_get_time();
                uint64_t dt = nt-st;
                frame_dur += dt;
                frame_buffer[frame_index++] = value;
                if(dt>20000){
                    printf("f:%d:%lld\n", frame_index, frame_dur);
                    for(int i = 0;i<frame_index;i++){
                        printf("d:%d\n", frame_buffer[i]);
                    }
                    //printf("\n");
                    frame_index = 0;
                    frame_dur = 0;
                }
                st = nt;
                
                j += 1 + rlength;
                continue;

            case 0x86: ;
                uint16_t interval = (0|pdata[++j])<<8;
                interval |= pdata[++j];
                ESP_LOGI("PRINTERVAL", "%d", interval);
                continue;
                
            case 0x83: ;
                uint32_t waves[8] = {0}; // Delta, theta, low-alpha, high-alpha, low-beta, high-beta, low-gamma and mid-gamma
                for(int i = 0;i<8;i++){
                    waves[i] |= (pdata[++j] << 8);
                    waves[i] |= (pdata[++j] << 8);
                    waves[i] |= pdata[++j];
                }
                printf("waves\n\n");
                ESP_LOGI("ASIC_EEG_POWER",
                         "\nDelta: %d\n"
                         "Theta: %d\n"
                         "low-alpha: %d\n"
                         "high-alpha: %d\n"
                         "low-beta: %d\n"
                         "high-beta: %d\n"
                         "low-gamma: %d\n"
                         "mid-gamma %d\n"
                         , waves[0], waves[1], waves[2], waves[3],
                         waves[4], waves[5], waves[6], waves[7]);
                continue;

            case 0x55: // These two are reserved
            case 0xAA: 
                continue;

            case 0xE3:
                continue;
                
            default:
                ESP_LOGE("Unkown command", "%2x", pdata[j]);
                printf("r:%d\n", ERROR_UNKOWN_CMD);
                break;
            }
        }
        fflush(stdout);
    }
}
