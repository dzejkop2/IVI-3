#include "button.h"

Button::Button(bool button)
{
    pressed = button;
    gpio_reset_pin(GPIO_NUM_13);
    gpio_set_direction(GPIO_NUM_13, GPIO_MODE_INPUT);
    gpio_pullup_en(GPIO_NUM_13);
}

bool Button::check()
{
    if(!gpio_get_level(GPIO_NUM_13))
    {
        if(!pressed)
        {
            pressed = true;  
            return true;
        }
        else return false;
    }
    else 
    {
        pressed = false;
        return false;
    }
}