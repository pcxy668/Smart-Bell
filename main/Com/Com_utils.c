#include "Com_utils.h"

// 毫秒级延时
void DelayMs(uint32_t time) {
    vTaskDelay(time / portTICK_PERIOD_MS);
}

// 微秒级延时
void DelayUs(uint32_t time)
{
    usleep(time);
}
