#ifndef __INT_LCD_H__
#define __INT_LCD_H__

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "jpeg_decoder.h"

//Define the height and width of the jpeg file. Make sure this matches the actual jpeg
//dimensions.
#define IMAGE_W 320
#define IMAGE_H 240

//Reference the binary-included jpeg file
extern const uint8_t image_jpg_start[] asm("_binary_image_jpg_start");
extern const uint8_t image_jpg_end[] asm("_binary_image_jpg_end");
extern uint8_t *pixels_buff;

esp_err_t Int_LCD_JPG2RGB565(uint8_t * jpeg_data, uint32_t size, uint8_t * outdata);
void Int_LCD_Init(void);
void Int_LCD_ShowImage(uint8_t *image);
void Int_LCD_Test(void);

#endif /* __INT_LCD_H__ */
