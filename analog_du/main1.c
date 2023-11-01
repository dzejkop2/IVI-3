/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <math.h>
#include "driver/gpio.h"
const static char *TAG = "EXAMPLE";

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/

#define CHANNEL          ADC_CHANNEL_0


#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_6

int maxvoltage[] = {950, 1250, 1750, 2450};

const gpio_num_t PIN1 = GPIO_NUM_4;
const gpio_num_t PIN2 = GPIO_NUM_5;
const gpio_num_t PIN3 = GPIO_NUM_18;
const gpio_num_t PIN4 = GPIO_NUM_19;
const gpio_num_t PIN5 = GPIO_NUM_21;
const gpio_num_t PIN6 = GPIO_NUM_22;
const gpio_num_t PIN7 = GPIO_NUM_23;
const gpio_num_t PIN8 = GPIO_NUM_13;
const gpio_num_t piny[] = {PIN1,PIN2,PIN3,PIN4,PIN5,PIN6,PIN7,PIN8};

void board_config()
{
    for (uint8_t i = 0; i < sizeof(piny) / sizeof(piny[0]); i++)
    {
        gpio_reset_pin(piny[i]);
        gpio_set_direction(piny[i],GPIO_MODE_OUTPUT);
        gpio_set_level(piny[i], 0);
    }
}

uint8_t get_state(int out)
{
    int max_out = 4095;
    if(out == max_out) return 9;
    else if (out >= (max_out / 8) * 7) return 8;
    else if(out >= (max_out / 8) * 6) return 7;
    else if(out >= (max_out / 8) * 5) return 6;
    else if(out >= (max_out / 8) * 4) return 5;
    else if(out >= (max_out / 8) * 3) return 4;
    else if(out >= (max_out / 8) * 2) return 3;
    else if(out >= max_out / 8) return 2;
    else return 1;
}

void check_state(int out)
{
    switch (get_state(out)) {
        case 1:
            gpio_set_level(PIN1,0);
            gpio_set_level(PIN2,0);
            gpio_set_level(PIN3,0);
            gpio_set_level(PIN4,0);
            gpio_set_level(PIN5,0);
            gpio_set_level(PIN6,0);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 2:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,0);
            gpio_set_level(PIN3,0);
            gpio_set_level(PIN4,0);
            gpio_set_level(PIN5,0);
            gpio_set_level(PIN6,0);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 3:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,0);
            gpio_set_level(PIN4,0);
            gpio_set_level(PIN5,0);
            gpio_set_level(PIN6,0);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 4:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,1);
            gpio_set_level(PIN4,0);
            gpio_set_level(PIN5,0);
            gpio_set_level(PIN6,0);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 5:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,1);
            gpio_set_level(PIN4,1);
            gpio_set_level(PIN5,0);
            gpio_set_level(PIN6,0);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 6:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,1);
            gpio_set_level(PIN4,1);
            gpio_set_level(PIN5,1);
            gpio_set_level(PIN6,0);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 7:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,1);
            gpio_set_level(PIN4,1);
            gpio_set_level(PIN5,1);
            gpio_set_level(PIN6,1);
            gpio_set_level(PIN7,0);
            gpio_set_level(PIN8,0);
            break;
        case 8:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,1);
            gpio_set_level(PIN4,1);
            gpio_set_level(PIN5,1);
            gpio_set_level(PIN6,1);
            gpio_set_level(PIN7,1);
            gpio_set_level(PIN8,0);
            break;
        case 9:
            gpio_set_level(PIN1,1);
            gpio_set_level(PIN2,1);
            gpio_set_level(PIN3,1);
            gpio_set_level(PIN4,1);
            gpio_set_level(PIN5,1);
            gpio_set_level(PIN6,1);
            gpio_set_level(PIN7,1);
            gpio_set_level(PIN8,1);
            break;
    }   
}

void app_main(void)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, CHANNEL, &config));
    board_config();

    while (1) {
        int out = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, CHANNEL, &out));
        //ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, CHANNEL, out);
        check_state(out);
        //double voltage = (double)out * maxvoltage[EXAMPLE_ADC_ATTEN]  / pow(2, config.bitwidth);
        vTaskDelay(pdMS_TO_TICKS(100));
        //ESP_LOGW("Voltage:"," %.2f mV", voltage);
    }
}