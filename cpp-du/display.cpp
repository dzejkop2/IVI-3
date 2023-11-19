#include "display.h"

const unsigned char numbers[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1}
};



Display::Display(gpio_num_t pins[])
{
    for (uint8_t i = 0; i < 7; i++)
    {
        leds[i] = Led(pins[i]);
    }
}

void Display::reset()
{
    for (uint8_t i = 0; i < 7; i++)
    {
        leds[i].off();
    }
}

void Display::display(uint8_t cislo)
{
    reset();
    for (uint8_t i = 0; i < 7; i++)
    {
        if(numbers[cislo][i] == 1)
        {
            leds[i].on();
        }
        else leds[i].off();
    }   
}