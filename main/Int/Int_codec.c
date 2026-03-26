#include "Int_codec.h"

i2c_master_bus_handle_t i2c0_bus_handle;

i2s_chan_handle_t i2s0_tx_handle;
i2s_chan_handle_t i2s0_rx_handle;

esp_codec_dev_handle_t codec_dev;
esp_codec_dev_sample_info_t fs = {
    .sample_rate = 16000,
    .channel = 1,
    .bits_per_sample = 16,
};

static const char *TAG = "CODEC";

static void Int_Codec_I2c_Init(void)
{
    i2c_master_bus_config_t i2c_bus_config = {0};
    i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config.i2c_port = I2C_NUM_0;
    i2c_bus_config.scl_io_num = GPIO_NUM_1;
    i2c_bus_config.sda_io_num = GPIO_NUM_0;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;
    esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, &i2c0_bus_handle);   
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "I2C init failed");
    }
}

static void Int_Codec_I2s_Init(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.auto_clear_after_cb = true;
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(16, I2S_SLOT_MODE_MONO),
        .gpio_cfg ={
            .mclk = GPIO_NUM_3,
            .bclk = GPIO_NUM_2,
            .ws = GPIO_NUM_5,
            .dout = GPIO_NUM_6,
            .din = GPIO_NUM_4,
        },
    };

    i2s_new_channel(&chan_cfg,&i2s0_tx_handle,&i2s0_rx_handle);

    i2s_channel_init_std_mode(i2s0_tx_handle, &std_cfg);
    i2s_channel_init_std_mode(i2s0_rx_handle, &std_cfg);

    esp_err_t ret = i2s_channel_enable(i2s0_tx_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "I2S_TX enable failed");
    }

    ret = i2s_channel_enable(i2s0_rx_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "I2S_RX enable failed");
    }
}

void Int_Codec_Init(void)
{
    Int_Codec_I2c_Init();
    Int_Codec_I2s_Init();

    //为编解码器设备实现控制接口，数据接口和 GPIO 接口 (使用默认提供的接口实现)
    audio_codec_i2s_cfg_t i2s_cfg = {
        .rx_handle = i2s0_rx_handle,
        .tx_handle = i2s0_tx_handle
    };
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = I2C_NUM_0,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c0_bus_handle
    };
    const audio_codec_ctrl_if_t *out_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    //基于控制接口和 ES8311 特有的配置实现 audio_codec_if_t 接口
    es8311_codec_cfg_t es8311_cfg = {
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,
        .ctrl_if = out_ctrl_if,
        .gpio_if = gpio_if,
        .pa_pin = GPIO_NUM_7,
        .use_mclk = true
    };
    const audio_codec_if_t *out_codec_if = es8311_codec_new(&es8311_cfg);    
    //通过API esp_codec_dev_new获取esp_codec_dev_handle_t句柄,用获取到的句柄来进行播放和录制操作
    esp_codec_dev_cfg_t dev_cfg = {
        .codec_if = out_codec_if,              // es8311_codec_new 获取到的接口实现
        .data_if = data_if,                    // audio_codec_new_i2s_data 获取到的数据接口实现
        .dev_type = ESP_CODEC_DEV_TYPE_IN_OUT  // 设备同时支持录制和播放
    };
    codec_dev = esp_codec_dev_new(&dev_cfg);

    esp_codec_dev_set_out_vol(codec_dev, 60);
    esp_codec_dev_set_in_gain(codec_dev, 15);

    Int_Codec_Open();

    //测试
    int regVal = 0;
    esp_codec_dev_read_reg(codec_dev,0xFE,&regVal);
    printf(" Reg 0xFE value = %#x\n", regVal); //读设备信息，正常值应为0x11
}

void Int_Codec_Play(const uint8_t *data, int data_size)
{
    esp_codec_dev_write(codec_dev, (void *)data, data_size);
}

void Int_Codec_PlayDoorbell(void)
{ 
    Int_Codec_Play(doorbell_pcm_start, doorbell_pcm_end - doorbell_pcm_start);
}

void Int_Codec_Record(uint8_t *data, int len)
{
    esp_codec_dev_read(codec_dev, (void *)data, len);
}

void Int_Codec_Open(void)
{
    esp_codec_dev_open(codec_dev, &fs);
}

void Int_Codec_Close(void)
{
    esp_codec_dev_close(codec_dev);
}
