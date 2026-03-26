#include "Int_mqtt.h"

#define MQTT_URL "mqtt://192.168.0.164:1883"

static const char *TAG = "mqtt_example";

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "/doorbell/cmd", 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        msg_id = esp_mqtt_client_publish(client, "/doorbell/status", "{\"esp_mqtt\":\"ok\"}", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        // 解析JSON
        cJSON *root = cJSON_ParseWithLength(event->data,event->data_len);
        if (root == NULL)
        {
            ESP_LOGI(TAG, "message null");
            cJSON_Delete(root);
            break;
        }
        cJSON *cmd = cJSON_GetObjectItem(root,"cmd");
        if (cmd == NULL)
        {
            ESP_LOGI(TAG, "message wrong");
            cJSON_Delete(root);
            break;
        }
        if (strcmp(cmd->valuestring,"switch") == 0)
        {
            Int_Websocket_Switch();
        }
        cJSON_Delete(root);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void Int_MQTT_Init(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_URL,
    }; 
    
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);    
}