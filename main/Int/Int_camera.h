#ifndef __INT_CAMERA_H__
#define __INT_CAMERA_H__

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"
#include "Int_lcd.h"

void Int_Camera_Init(void);
void Int_Camera_Capture(void);

#endif /* __INT_CAMERA_H__ */
