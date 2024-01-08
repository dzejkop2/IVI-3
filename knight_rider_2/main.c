#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include <time.h>

const gpio_num_t LED_PIN_1 = GPIO_NUM_4;
const gpio_num_t LED_PIN_2 = GPIO_NUM_5;
const gpio_num_t LED_PIN_3 = GPIO_NUM_18;
const gpio_num_t LED_PIN_4 = GPIO_NUM_19;
const gpio_num_t LED_PIN_5 = GPIO_NUM_21;
const gpio_num_t LED_PIN_6 = GPIO_NUM_22;
const gpio_num_t LED_PIN_7 = GPIO_NUM_23;
const gpio_num_t LED_PIN_8 = GPIO_NUM_13;
#define LEDC_MODE LEDC_LOW_SPEED_MODE
const int max = 4095;
const int half = max/2;
const int quarter = max/4;
const int t_quarter = quarter*3;

uint32_t displayStartTime = 0;
const uint32_t display_time = 95;
uint8_t count = 0;

const gpio_num_t piny[] = {LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5, LED_PIN_6, LED_PIN_7, LED_PIN_8};
const uint8_t channel[] = {0,1,2,3,4,5,6,7};
const uint8_t delay = 7;
bool opakovanie = false;

enum State 
{
    STATE_1,
    STATE_2,
    STATE_3,
    STATE_4,
    STATE_5,
    STATE_6
};

enum State current_state = STATE_1;

void clean()
{
    for (uint8_t i = 0; i < sizeof(piny) / sizeof(piny[0]); i++)
    {
        ledc_set_duty(LEDC_MODE,channel[i],0);
        ledc_update_duty(LEDC_MODE, channel[i]);
    }
    
}

void reset_time()
{
    displayStartTime = (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC);
}

void display()
{
    uint32_t currentTime = (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC);
    uint32_t timeDiff = currentTime - displayStartTime;
    switch (current_state)
    {
        case STATE_1:
            if (display_time < timeDiff)
            {
                if(!opakovanie)
                {
                    clean();
                    for (uint8_t i = 0; i < 3; i++)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                else
                {
                    clean();
                    for (uint8_t i = 7; i > 4; i--)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                reset_time();
                current_state = STATE_2;
            }
            break;
        
        case STATE_2:
            if (display_time < timeDiff)
            {
                if(!opakovanie)
                {
                    clean();
                    ledc_set_duty(LEDC_MODE,channel[0],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[0]);
                    for (uint8_t i = 1; i < 4; i++)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                else 
                {
                    clean();
                    ledc_set_duty(LEDC_MODE,channel[7],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[7]);
                    for (uint8_t i = 6; i > 3; i--)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                reset_time();
                current_state = STATE_3;
            }
            break;
        case STATE_3:
            if (display_time < timeDiff)
            {
                if(!opakovanie)
                {
                    clean();
                    ledc_set_duty(LEDC_MODE,channel[0],half);
                    ledc_update_duty(LEDC_MODE, channel[0]);
                    ledc_set_duty(LEDC_MODE,channel[1],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[1]);
                    for (uint8_t i = 2; i < 5; i++)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                else
                {
                    clean();
                    ledc_set_duty(LEDC_MODE,channel[7],half);
                    ledc_update_duty(LEDC_MODE, channel[7]);
                    ledc_set_duty(LEDC_MODE,channel[6],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[6]);
                    for (uint8_t i = 5; i > 2; i--)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                reset_time();
                current_state = STATE_4;
            }
            break;
        case STATE_4:
            if (display_time < timeDiff)
            {
                if(!opakovanie)
                {
                    clean();
                    ledc_set_duty(LEDC_MODE,channel[0 + count],quarter);
                    ledc_update_duty(LEDC_MODE, channel[0 + count]);
                    ledc_set_duty(LEDC_MODE,channel[1 + count],half);
                    ledc_update_duty(LEDC_MODE, channel[1 + count]);
                    ledc_set_duty(LEDC_MODE,channel[2 + count],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[2 + count]);
                    for (uint8_t i = 3 + count; i < 6 + count; i++)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                else
                {
                    clean();
                    ledc_set_duty(LEDC_MODE,channel[7 - count],quarter);
                    ledc_update_duty(LEDC_MODE, channel[7 - count]);
                    ledc_set_duty(LEDC_MODE,channel[6 - count],half);
                    ledc_update_duty(LEDC_MODE, channel[6 - count]);
                    ledc_set_duty(LEDC_MODE,channel[5 - count],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[5 - count]);
                    for (int8_t i = 4 - count; i > 1 - count; i--)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                    
                }
                reset_time();
                count++;
                if(count == 2)
                {
                    current_state = STATE_5;
                    count = 0;
                }
            }
            break;
        case STATE_5:
            if (display_time < timeDiff)
            {
                if(!opakovanie)
                {
                    clean();
                    ledc_set_duty(LEDC_MODE, channel[4],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[4]);
                    ledc_set_duty(LEDC_MODE, channel[3], half);
                    ledc_update_duty(LEDC_MODE, channel[3]);
                    for (uint8_t i = 7; i > 4; i--)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                }
                else 
                {
                    clean();
                    for (uint8_t i = 0; i < 3; i++)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                    ledc_set_duty(LEDC_MODE, channel[3],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[3]);
                    ledc_set_duty(LEDC_MODE, channel[4], half);
                    ledc_update_duty(LEDC_MODE, channel[4]);
                }
                reset_time();
                current_state = STATE_6;
            }
            break;

        case STATE_6:
            if (display_time < timeDiff)
            {
                if(!opakovanie)
                {
                    clean();
                    ledc_set_duty(LEDC_MODE, channel[4],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[4]);
                    for (uint8_t i = 7; i > 4; i--)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                    opakovanie = true;
                }
                else 
                {
                    clean();
                    for (uint8_t i = 0; i < 3; i++)
                    {
                        ledc_set_duty(LEDC_MODE,channel[i],max);
                        ledc_update_duty(LEDC_MODE, channel[i]);
                    }
                    ledc_set_duty(LEDC_MODE, channel[3],t_quarter);
                    ledc_update_duty(LEDC_MODE, channel[3]);
                    opakovanie = false;
                }
                reset_time();
                current_state = STATE_1;
            }
            break;
    }
}

void app_main() {
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    ledc_channel_config_t ledc_channel[8] = {
        {
            .channel    = 0,
            .duty       = 0,
            .gpio_num   = LED_PIN_1,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 1,
            .duty       = 0,
            .gpio_num   = LED_PIN_2,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 2,
            .duty       = 0,
            .gpio_num   = LED_PIN_3,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 3,
            .duty       = 0,
            .gpio_num   = LED_PIN_4,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 4,
            .duty       = 0,
            .gpio_num   = LED_PIN_5,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 5,
            .duty       = 0,
            .gpio_num   = LED_PIN_6,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 6,
            .duty       = 0,
            .gpio_num   = LED_PIN_7,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = 7,
            .duty       = 0,
            .gpio_num   = LED_PIN_8,
            .speed_mode = LEDC_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        }
    };
    for (uint8_t i = 0; i < sizeof(ledc_channel) / sizeof(ledc_channel[0]); i++)
    {
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[i]));
    }

    while (1) 
    {
        display(opakovanie);
        vTaskDelay(1);
    }
}   