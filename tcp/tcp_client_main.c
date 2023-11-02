/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "tcp_client.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <string.h>


extern void tcp_client(void);

int sock = 0;
bool button_pressed = false;
bool state = false;

static const char *TAG = "MAIN";

void board_config()
{
    gpio_reset_pin(GPIO_NUM_4);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 0);
    gpio_reset_pin(GPIO_NUM_5);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    gpio_pullup_en(GPIO_NUM_5);
}

void recieve(void *arg)
{
    while(1)
    {
        char data_buffer[1000] = {0};
        bool rec_status = tcp_client_receive(sock, data_buffer, sizeof(data_buffer));    
        if (rec_status)
        {
            ESP_LOGI(TAG, "Received data - %s", data_buffer);
            if(strcmp(data_buffer, "LED=ON") == 0)
            {
                state = true;
                gpio_set_level(GPIO_NUM_4,1);
            }  
            else if(strcmp(data_buffer,"LED=OFF") == 0)
            {
                state = false;
                gpio_set_level(GPIO_NUM_4, 0);
            }
            else if(strcmp(data_buffer, "LED=TOGGLE") == 0)
            {
                state = !state;
                gpio_set_level(GPIO_NUM_4, state);
            }
        }
        else
        {
            ESP_LOGW(TAG, "There was some problem during receiving of data");
        }
        vTaskDelay(100);
    }
}

void send(void *arg)
{
    while(1)
    {
        if(!gpio_get_level(GPIO_NUM_5))
        {
            if(!button_pressed)
            {
                if(state == true)
                {
                    gpio_set_level(GPIO_NUM_4, 0);
                    state = !state;
                }
                else 
                {
                    gpio_set_level(GPIO_NUM_4, 1);
                    state = !state;
                }
                button_pressed = true;
                tcp_client_send(sock, "Button bol pushnuty\n");
            }
        }
        else button_pressed = false;
        vTaskDelay(10);
    }
}

void app_main(void)
{
    board_config();

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    do
    {
        sock = tcp_client_init("192.168.0.106", 8075);
    } while (sock <= 0);
    xTaskCreate(send, "send", 4096, NULL, 10, NULL);
    xTaskCreate(recieve, "recieve", 4096, NULL, 10, NULL);
}
