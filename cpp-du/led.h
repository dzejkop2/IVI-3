#pragma once

#include "driver/gpio.h"

class Led
{
    private:
        gpio_num_t pin_led = GPIO_NUM_0;
    
    public:
        Led(gpio_num_t pin);
        void on();
        void off();
};