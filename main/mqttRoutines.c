#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"   
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_timer.h"
#include "esp_ota_ops.h"
#include "driver/uart.h"
#include "esp_netif.h"
#include "rom/ets_sys.h"
#include "esp_smartconfig.h"
#include <sys/socket.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "mqtt_client.h"

#include "calls.h"

static const char *TAG = "MQTT";

void InitMqtt (void);


int32_t MQTT_CONNEECTED = 1;
char SerialNumber[100];
char topic[356]="GVC/VM/";

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */


 esp_mqtt_client_handle_t client = NULL;

 


void publish_message(const char *message, esp_mqtt_client_handle_t client) {
    // Publish the provided message to the MQTT topic
    esp_mqtt_client_publish(client, "GVC/VM/ALL", message, strlen(message), 0, 0);

    // Indicate that a transaction is pending
    // tx_event_pending = 1;

    // Log the published message for debugging
    // ESP_LOGI(TAG, "Published SIP message: %s", message);
}

void publish_mqtt_message(const char *message){
    publish_message(message, client);
}

void Publisher_Task(void *params)
{
  while (1)
  {
    // ESP_LOGI(TAG,"*Sending HBT from Device#");
    if(MQTT_CONNEECTED)
    {
        publish_message("*HBT#", client);
        // ESP_LOGI(TAG,"*HBT Sent#");
       
    }
    vTaskDelay(15000 / portTICK_PERIOD_MS);
  }
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, (int)event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    // Declare variables outside the switch statement
   
    char data[256];  // Assuming a max data length
    char payload[500];
    char buf[500];

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNEECTED = 1;  // Ensure MQTT_CONNECTED is defined
        
      
        
        sprintf (payload,"Topic is %s",topic);
        uart_write_string_ln(payload);
        msg_id = esp_mqtt_client_subscribe(client, topic, 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNEECTED = 0;
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
        ESP_LOGI(TAG,"TOPIC=%.*s\r\n", event->topic_len, event->topic);
        ESP_LOGI(TAG,"DATA=%.*s\r\n", event->data_len, event->data);

        // Ensure topic and data are within bounds
        if (event->topic_len < sizeof(topic) && event->data_len < sizeof(data)) {
            strncpy(topic, event->topic, event->topic_len);
            topic[event->topic_len] = '\0';

            strncpy(data, event->data, event->data_len);
            data[event->data_len] = '\0';

            char expected_topic[150];
            sprintf(expected_topic, "GVC/VM/%s", SerialNumber);

            // if (strcmp(topic, expected_topic) == 0) {
                // write here commands
            // }
        } else {
            ESP_LOGE(TAG, "Received topic/data too large");
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        // Add more error handling here if needed
        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}


void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "STARTING MQTT");
    esp_mqtt_client_config_t mqttConfig = {
         .broker.address.uri = "mqtt://zest-iot.in:1883",
        .session.protocol_ver = MQTT_PROTOCOL_V_3_1_1,
        .network.disable_auto_reconnect = true,
        .credentials.username = "123",
        .credentials.authentication.password = "456",
        .session.last_will.topic = "GVC/VM/00002",
        .session.last_will.msg = "i will leave",
        .session.last_will.msg_len = 12,
        .session.last_will.qos = 1,
        .session.last_will.retain = true,};
    
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}





void InitMqtt (void)
{
     xTaskCreate(Publisher_Task, "Publisher_Task", 1024 * 5, NULL, 5, NULL);
    
     ESP_LOGI(TAG,"MQTT Sending HBT Initiated ");
}