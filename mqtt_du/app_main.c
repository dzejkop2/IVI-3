#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static const char *TAG = "MQTT_EXAMPLE";
bool led_state = false;

bool button_pressed = false;
esp_mqtt_client_handle_t client;
adc_oneshot_unit_handle_t adc1_handle;


void board_config()
{
    gpio_reset_pin(GPIO_NUM_5);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ONLY);
}

void check_button()
{
    while(1)
    {
        if(!gpio_get_level(GPIO_NUM_5))
        {
            if(!button_pressed)
            {
                if(led_state == true)
                {
                    led_state = false;
                    esp_mqtt_client_publish(client, "/marioDzejkop/smart/light/onOff", "0", 0, 0, 1);
                }
                else 
                {
                    led_state = true;
                    esp_mqtt_client_publish(client, "/marioDzejkop/smart/light/onOff", "1", 0, 0, 1);
                }
                button_pressed = true;
            }
        }
        else button_pressed = false;
        vTaskDelay(pdMS_TO_TICKS(10));
    }   
}

char *get_val(int out, int max_out)
{
    if(out == max_out) return "8";
    else if (out >= (max_out / 8) * 7) return "7";
    else if(out >= (max_out / 8) * 6) return "6";
    else if(out >= (max_out / 8) * 5) return "5";
    else if(out >= (max_out / 8) * 4) return "4";
    else if(out >= (max_out / 8) * 3) return "3";
    else if(out >= (max_out / 8) * 2) return "2";
    else if(out >= max_out / 8) return "1";
    else return "0";
}

int divide(int num, int max)
{
    return (num + 1) / (max/ 8);
}

void check_pot()
{
    int led_prog = 0;
    int prev_led_prog = 0;
    while(1)
    {
        prev_led_prog = led_prog;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &led_prog);
        if(divide(led_prog,512) != divide(prev_led_prog,512))
        {
            esp_mqtt_client_publish(client, "/marioDzejkop/smart/light/brightness", get_val(divide(led_prog,512), 8), 0, 0, 1);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "/marioDzejkop/smart/light/brightness", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, "/marioDzejkop/smart/light/onOff", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if(strncmp("/marioDzejkop/smart/light/onOff", event->topic, event->topic_len) == 0)
        {
            if(strncmp("0",event->data,event->data_len) == 0)
            {
                led_state = false;
            }
            else if(strncmp("1",event->data,event->data_len) == 0)
            {
                led_state = true;
            }
        }
        
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_9,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle,ADC_CHANNEL_0, &config));
   
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    board_config();

    ESP_ERROR_CHECK(example_connect());
    
    mqtt_app_start();

    xTaskCreate(check_button, "button_task", 2048, NULL, 10, NULL);
    xTaskCreatePinnedToCore(check_pot, "pot_task", 2048, NULL, 10, NULL, 1);
}