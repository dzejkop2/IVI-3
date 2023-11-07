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
#include "esp_vfs.h"


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

int socket_create()
{
    int new_sock;
    do
    {
        new_sock = tcp_client_init("192.168.1.23", 8075);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while (new_sock <= 0);
    return new_sock;
}

void app_main(void)
{
    gpio_reset_pin(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2,GPIO_MODE_OUTPUT);

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

    sock = socket_create();

    while(1)
    {   
        int out = 0;
        char data_buffer[1000] = {0};

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &out));
        bool send_status = tcp_client_send(sock,get_state(out));
        if (send_status)
        {
            bool rec_status = tcp_client_receive(sock, data_buffer, sizeof(data_buffer));
            if (rec_status)
            {
                gpio_set_level(GPIO_NUM_2,1);
                ESP_LOGI(TAG, "%s", data_buffer);
            }
            else
            {
                gpio_set_level(GPIO_NUM_2, 0);
                continue;
            }
        }
        else 
        {            
            gpio_set_level(GPIO_NUM_2, 0);
            close(sock);
            sock = -1;
            sock = socket_create();
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}