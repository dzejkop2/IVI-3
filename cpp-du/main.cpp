#include <stdio.h>
#include "button.h"
#include "led.h"
#include "display.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


extern "C"
{
    void app_main(void)
    {
        bool button = false;
        Button btn(button);
        uint8_t cislo = 0;
        gpio_num_t pins[7] = {GPIO_NUM_4,GPIO_NUM_5,GPIO_NUM_23,GPIO_NUM_22,GPIO_NUM_21,GPIO_NUM_18,GPIO_NUM_19};

        Display display(pins);
        display.display(cislo);

        while(1)
        {
            if(btn.check())
            {
                cislo++;
                if(cislo > 9)
                {
                    cislo = 0;
                }
                display.display(cislo);
            }
            vTaskDelay(10);
        }
    }
}