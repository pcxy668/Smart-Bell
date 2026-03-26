#ifndef __COM_UTILS_H__
#define __COM_UTILS_H__

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unistd.h"

void DelayMs(uint32_t time);
void DelayUs(uint32_t time);

#endif /* __COM_UTILS_H__ */
