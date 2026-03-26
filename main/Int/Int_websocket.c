#include "Int_websocket.h"

#define WEBSOCKET_BASE_URL "ws://192.168.0.164:8000"

esp_websocket_client_handle_t ws_image_client;
esp_websocket_client_handle_t ws_audio_client;
EventGroupHandle_t bell_event_group = NULL;

static const char *TAG = "websocket";

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)handler_args;
    switch (event_id) {
    case WEBSOCKET_EVENT_BEGIN:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_BEGIN");
        break;
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        if (data->op_code == 0x2 && client == ws_audio_client) { // Opcode 0x2 indicates binary data
            ESP_LOGI(TAG, "Received binary data");
            Int_Codec_Play((uint8_t *)(data->data_ptr), data->data_len);
        } else if (data->op_code == 0x08 && data->data_len == 2) {
            ESP_LOGW(TAG, "Received closed message with code=%d", 256 * data->data_ptr[0] + data->data_ptr[1]);
        } else {
            ESP_LOGW(TAG, "Received=%.*s\n\n", data->data_len, (char *)data->data_ptr);
        }
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    case WEBSOCKET_EVENT_FINISH:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_FINISH");
        break;
    }
}

void Int_Websocket_Init(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");

    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.uri = WEBSOCKET_BASE_URL "/ws/esp_image";
    ws_image_client = esp_websocket_client_init(&websocket_cfg);

    websocket_cfg.uri = WEBSOCKET_BASE_URL "/ws/esp_audio";
    ws_audio_client = esp_websocket_client_init(&websocket_cfg);

    esp_websocket_register_events(ws_image_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)ws_image_client);    
    esp_websocket_register_events(ws_audio_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)ws_audio_client);    
}

void Int_Websocket_Start(void)
{
    esp_websocket_client_start(ws_audio_client);
    esp_websocket_client_start(ws_image_client);
}

void Int_Websocket_Close(void)
{
    esp_websocket_client_close(ws_audio_client,1000);
    esp_websocket_client_close(ws_image_client,1000);
}

void Int_Websocket_Switch(void)
{
    EventBits_t event_bits = xEventGroupGetBits(bell_event_group);
    if (event_bits & WS_EVENT_BIT)
    {
        // 关闭WebSocket连接
        Int_Websocket_Close();
        xEventGroupClearBits(bell_event_group, WS_EVENT_BIT);
    }
    else
    {
        // 开启WebSocket连接
        Int_Websocket_Start();
        xEventGroupSetBits(bell_event_group, WS_EVENT_BIT);
    }
}

bool Int_Websocket_IsImageConnected(void)
{
    return esp_websocket_client_is_connected(ws_image_client);
}

bool Int_Websocket_IsAudioConnected(void)
{
    return esp_websocket_client_is_connected(ws_audio_client);
}

void Int_Websocket_SendAudio(uint8_t *data, int len)
{
    esp_websocket_client_send_bin(ws_audio_client, (char *)data, len, 1000);
}

void Int_Websocket_SendImage(uint8_t *data, int len)
{
    esp_websocket_client_send_bin(ws_image_client, (char *)data, len, 1000);
}
