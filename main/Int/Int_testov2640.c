/**
 * ov2640 new I2C driver通讯测试程序
 */
#include "Int_testov2640.h"

extern i2c_master_bus_handle_t i2c0_bus_handle;
void testov2640_init(void)
{
    // i2c_master_bus_handle_t i2c_temp_bus_handle;
    i2c_master_dev_handle_t dev_handle;

    // i2c_master_bus_config_t i2c_bus_config = {0};
    // i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    // i2c_bus_config.i2c_port = I2C_NUM_0;
    // i2c_bus_config.scl_io_num = GPIO_NUM_1;
    // i2c_bus_config.sda_io_num = GPIO_NUM_0;
    // i2c_bus_config.glitch_ignore_cnt = 7;
    // i2c_bus_config.flags.enable_internal_pullup = true;
    // esp_err_t ret = i2c_new_master_bus(&i2c_bus_config, &i2c_temp_bus_handle);   
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGI("test ov2640", "I2C init failed");
    // }
    ESP_LOGI("test ov2640", "I2C init"); 
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x30,
        .scl_speed_hz = 100000,
    };    
    i2c_master_bus_add_device(i2c0_bus_handle,&dev_cfg,&dev_handle);
    uint8_t writebuff[2] = {0x1C,0x00};
    uint8_t readbuff[2] = {0};
    ESP_LOGI("test ov2640", "send cmd");
    i2c_master_transmit_receive(dev_handle,writebuff,1,readbuff,1,1000);
    ESP_LOGI("test ov2640", "receive %#x",readbuff[0]);
    writebuff[0] = 0x1D;
    i2c_master_transmit_receive(dev_handle,writebuff,1,readbuff,1,1000);
    ESP_LOGI("test ov2640", "receive %#x",readbuff[0]);
}
