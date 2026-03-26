#include <stdio.h>
#include "Int_button.h"
#include "Int_led.h"
#include "Int_codec.h"
#include "Int_wifi.h"
#include "Int_lcd.h"
#include "Int_camera.h"
#include "Int_mqtt.h"
#include "Int_websocket.h"
#include "Com_utils.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

uint8_t audio_buff[2048];
camera_fb_t *pic;
SemaphoreHandle_t lcd_get_semaphore;
SemaphoreHandle_t lcd_finish_semaphore;

// 音频推送任务
void audio_task(void *pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(bell_event_group, WS_EVENT_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        if (Int_Websocket_IsAudioConnected())
        {
            ESP_LOGI("audio_task", "send audio");
            Int_Codec_Record(audio_buff,2048);
            Int_Websocket_SendAudio(audio_buff,2048);
        }
        else
        {
            DelayMs(50);
        }
    } 
}

// 视频推送任务(用于向Websocket发送)
void image_task(void *pvParameters)
{
    while (1)
    {
        EventBits_t bit = xEventGroupWaitBits(bell_event_group, WS_EVENT_BIT | LCD_EVENT_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

        pic = esp_camera_fb_get();

        // 验证JPEG格式头尾
        if (pic->len < 4 || pic->buf[0] != 0xFF || pic->buf[1] != 0xD8 || pic->buf[pic->len-2] != 0xFF || pic->buf[pic->len-1] != 0xD9)
        {
            ESP_LOGE("image_task", "Invalid JPEG format");
            esp_camera_fb_return(pic);   
            continue;     
        }  
        
        if (bit & LCD_EVENT_BIT)
        {
            xSemaphoreGive(lcd_get_semaphore);
        }
        
        if (bit & WS_EVENT_BIT)
        {
            Int_Websocket_SendImage(pic->buf, pic->len);
        }        

        if (bit & LCD_EVENT_BIT)
        {
            xSemaphoreTake(lcd_finish_semaphore, 200);
        }

        esp_camera_fb_return(pic);
    }
}

// 视频推送子任务(用于向LCD发送，与websocket并行以提高效率)
void image_slave_task(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(lcd_get_semaphore, portMAX_DELAY);
        
        esp_err_t ret = Int_LCD_JPG2RGB565(pic->buf, pic->len, pixels_buff); 
        if (ret == ESP_OK)
        {
            Int_LCD_ShowImage(pixels_buff);
        }       
        
        xSemaphoreGive(lcd_finish_semaphore);
    }
}

void app_main(void)
{
    //================= 创建任务通信量 =================
    bell_event_group = xEventGroupCreate();
    lcd_get_semaphore = xSemaphoreCreateBinary();
    lcd_finish_semaphore = xSemaphoreCreateBinary();

    //==================== 外设初始化 ====================
    Int_Button_Init();
    Int_WS2812B_Init();
    Int_Codec_Init();
    Int_WIFI_Init();
    Int_LCD_Init();
    Int_LCD_Test();
    DelayMs(2000);
    Int_Camera_Init();
    Int_MQTT_Init();
    Int_Websocket_Init();

    //==================== 任务调度 ====================
    xTaskCreate(audio_task, "audio_task", 4096, NULL, 3, NULL);
    xTaskCreate(image_task, "image_task", 4096, NULL, 3, NULL);
    xTaskCreate(image_slave_task, "image_slave_task", 4096, NULL, 3, NULL);
}
