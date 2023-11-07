#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "tcp_client.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"


int sock = 0;
bool button_pressed = false;
bool state = false;

static const char *TAG = "MAIN";

char *get_state(int out)
{
    int max_out = 4095;
    if(out == max_out) return "STATE=8";
    else if (out >= (max_out / 8) * 7) return "STATE=7";
    else if(out >= (max_out / 8) * 6) return "STATE=6";
    else if(out >= (max_out / 8) * 5) return "STATE=5";
    else if(out >= (max_out / 8) * 4) return "STATE=4";
    else if(out >= (max_out / 8) * 3) return "STATE=3";
    else if(out >= (max_out / 8) * 2) return "STATE=2";
    else if(out >= max_out / 8) return "STATE=1";
    else return "STATE=0";
}

void app_main(void)
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_6,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());
    do
    {
        sock = tcp_client_init("192.168.1.23", 8075);
    } while (sock <= 0);
    
    while(1)
    {   
        int out = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &out));
        tcp_client_send(sock,get_state(out));
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}