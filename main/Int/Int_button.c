#include "Int_button.h"

#define TAG "Button"

button_handle_t adc_btn1 = NULL;
button_handle_t adc_btn2 = NULL;

static void Int_Button1_Callback(void *arg,void *usr_data)
{
    if ((button_event_t)usr_data == BUTTON_SINGLE_CLICK)
    {
        ESP_LOGI(TAG, "BUTTON1_SINGLE_CLICK");
        Int_WS2812B_Light(0);
        Int_Codec_Play(blame_pcm_start, blame_pcm_end - blame_pcm_start);
    }
}

static void Int_Button2_Callback(void *arg,void *usr_data)
{
    if ((button_event_t)usr_data == BUTTON_SINGLE_CLICK)
    {
        ESP_LOGI(TAG, "BUTTON2_SINGLE_CLICK");
        Int_Codec_Play(myfault_pcm_start, myfault_pcm_end - myfault_pcm_start);

        EventBits_t event_bits = xEventGroupGetBits(bell_event_group);
        if (event_bits & LCD_EVENT_BIT)
        {
            // 关闭LCD输出标志位
            xEventGroupClearBits(bell_event_group, LCD_EVENT_BIT);
        }
        else
        {
            // 开启LCD输出标志位
            xEventGroupSetBits(bell_event_group, LCD_EVENT_BIT);
        }        
    }
    else if ((button_event_t)usr_data == BUTTON_LONG_PRESS_START)
    {
        Int_WIFI_Reset();
        ESP_LOGI(TAG, "BUTTON2_LONG_PRESS_START");
    }
}
void Int_Button_Init(void)
{
    //创建ADC按键1
    const button_config_t btn_cfg = {0};
    button_adc_config_t btn_adc_cfg = {
        .unit_id = ADC_UNIT_1,
        .adc_channel = 7,
        .button_index = 0,
        .min = 0,
        .max = 600,
    };

    iot_button_new_adc_device(&btn_cfg, &btn_adc_cfg, &adc_btn1);
    if(NULL == adc_btn1) {
        ESP_LOGE(TAG, "Button1 create failed");
    }

    //创建ADC按键2
    btn_adc_cfg.button_index = 1;
    btn_adc_cfg.min = 1200; 
    btn_adc_cfg.max = 2000;
    
    iot_button_new_adc_device(&btn_cfg, &btn_adc_cfg, &adc_btn2);
    if(NULL == adc_btn2) {
        ESP_LOGE(TAG, "Button2 create failed");
    }  

    //注册回调函数
    iot_button_register_cb(adc_btn1, BUTTON_SINGLE_CLICK, NULL, Int_Button1_Callback,(void *)BUTTON_SINGLE_CLICK);    
    iot_button_register_cb(adc_btn2, BUTTON_SINGLE_CLICK, NULL, Int_Button2_Callback,(void *)BUTTON_SINGLE_CLICK);    
    iot_button_register_cb(adc_btn2, BUTTON_LONG_PRESS_START, NULL, Int_Button2_Callback,(void *)BUTTON_LONG_PRESS_START);    
}