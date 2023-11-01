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
const static char *TAG = "EXAMPLE";

/*---------------------------------------------------------------
        ADC General Macrosxdxdxdxdxdxdxd
---------------------------------------------------------------*/

#define CHANNEL          ADC_CHANNEL_0


#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_6

int maxvoltage[] = {950, 1250, 1750, 2450};

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

    while (1) {
        int out = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, CHANNEL, &out));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, CHANNEL, out);
        double voltage = (double)out * maxvoltage[EXAMPLE_ADC_ATTEN]  / pow(2, config.bitwidth);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGW("Voltage:"," %.2f mV", voltage);
    }
}