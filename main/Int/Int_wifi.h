#ifndef __INT_WIFI_H__
#define __INT_WIFI_H__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "nvs_flash.h"


#include "lwip/err.h"
#include "lwip/sys.h"

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "qrcode.h"

void Int_WIFI_Init(void);
void Int_WIFI_Reset(void);

#endif /* __INT_WIFI_H__ */
