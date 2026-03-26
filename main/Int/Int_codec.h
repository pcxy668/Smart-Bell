#ifndef __INT_CODEC_H__
#define __INT_CODEC_H__

#include "driver/i2c_master.h"
#include "driver/i2s_std.h"

#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"

#include "esp_log.h"

extern const uint8_t doorbell_pcm_start[] asm("_binary_doorbell_pcm_start");
extern const uint8_t doorbell_pcm_end[]   asm("_binary_doorbell_pcm_end");
extern const uint8_t blame_pcm_start[] asm("_binary_blame_pcm_start");
extern const uint8_t blame_pcm_end[]   asm("_binary_blame_pcm_end");
extern const uint8_t myfault_pcm_start[] asm("_binary_myfault_pcm_start");
extern const uint8_t myfault_pcm_end[]   asm("_binary_myfault_pcm_end");

void Int_Codec_Init(void);
void Int_Codec_Play(const uint8_t *data, int data_size);
void Int_Codec_PlayDoorbell(void);
void Int_Codec_Record(uint8_t *data, int len);
void Int_Codec_Open(void);
void Int_Codec_Close(void);

#endif /* __INT_CODEC_H__ */
