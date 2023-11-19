#pragma once

#include "led.h"

class Display
{
    private:
        void reset(); 
        Led leds[7] = {GPIO_NUM_4,GPIO_NUM_4,GPIO_NUM_4, GPIO_NUM_4, GPIO_NUM_4, GPIO_NUM_4, GPIO_NUM_4};

    public:
        Display(gpio_num_t pins[]);
        void display(uint8_t cislo);
};