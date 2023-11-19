#include "led.h"

Led::Led(gpio_num_t pin)
{
    pin_led = pin;
    gpio_reset_pin(pin_led);
    gpio_set_direction(pin_led, GPIO_MODE_OUTPUT);
}

void Led::on()
{
    gpio_set_level(pin_led, 1);
}

void Led::off()
{
    gpio_set_level(pin_led, 0);
}