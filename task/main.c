#include <stdio.h>  
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/semphr.h"

/*
void task1()
{
    while(1)
    {
        ESP_LOGE("Task 1","1000ms");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2()
{
    while(1)
    {
        ESP_LOGI("Task 2","100ms");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void task3()
{
    while(1)
    {
        ESP_LOGW("Task 3","3000ms");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    xTaskCreate(task1, "Task_1", 2048, NULL, 1, NULL);
    xTaskCreate(task2, "Task_2", 2048, NULL, 2, NULL);
    xTaskCreate(task3, "Task_3", 2048, NULL, 3, NULL);
}
*/

SemaphoreHandle_t buttonSemaphore;
bool led_state = false;
bool button_pressed = false;

void board_config()
{
    gpio_reset_pin(GPIO_NUM_22);
    gpio_reset_pin(GPIO_NUM_23);
    gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_INPUT);
    gpio_pullup_en(GPIO_NUM_23);
}

void button_task(void *arg) 
{
    while (1) 
    {
        if (gpio_get_level(GPIO_NUM_23) == 0) 
        {
            if(!button_pressed)
            {
                xSemaphoreGive(buttonSemaphore);
                button_pressed = true;
            }
        }
        else button_pressed = false;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void led_task(void *arg) 
{
    while (1) 
    {
        if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY)) 
        {
            led_state = !led_state;
            gpio_set_level(GPIO_NUM_22, led_state);
        }
    }
}

void app_main() 
{
    board_config();
    buttonSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
    xTaskCreate(led_task, "led_task", 2048, NULL, 10, NULL);
}