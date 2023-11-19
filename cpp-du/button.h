#pragma once

#include "driver/gpio.h"

class Button
{
    private:
        bool pressed = false;
        
    public:
        Button(bool button);
        bool check();
};