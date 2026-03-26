#ifndef __INT_WEBSOCKET_H__
#define __INT_WEBSOCKET_H__

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "Int_codec.h"

#define WS_EVENT_BIT BIT0
#define LCD_EVENT_BIT BIT1

extern EventGroupHandle_t bell_event_group;

void Int_Websocket_Init(void);
void Int_Websocket_Start(void);
void Int_Websocket_Close(void);
void Int_Websocket_Switch(void);
bool Int_Websocket_IsImageConnected(void);
bool Int_Websocket_IsAudioConnected(void);
void Int_Websocket_SendAudio(uint8_t *data, int len);
void Int_Websocket_SendImage(uint8_t *data, int len);

#endif /* __INT_WEBSOCKET_H__ */
