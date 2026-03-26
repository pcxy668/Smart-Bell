# 智能门铃项目

### 1.目录结构 
* main/Com 公共层             
* main/Int 接口层
* main/main.c 主程序入口
* others 服务端测试程序，包括mqtt服务配置和Websocket服务及前端页面
### 2.硬件组成
* 主控芯片 ESP32-S3-WROOM-1-N16R8
* 音频编解码模块 ES8311+NS4150B+扬声器+数字麦克风 
* 摄像头 OV2640
* 显示屏 2.0寸ST7789VW驱动IPS屏（SPI通信）
* RGB三色灯 WS2818
### 3.项目说明
* 采用ADC按键，节约引脚数量
* 配网方式采用BLE配网，程序初次运行时，需配合ESP官方配网APP进行串口扫码配网，详见官方文档：https://docs.espressif.com/projects/esp-idf/zh_CN/v5.5.3/esp32/api-reference/provisioning/wifi_provisioning.html
* 按键1短按——门铃音效
* 按键2短按——屏幕显示实时摄像画面（可配自定义音效）
* 按键2长按——WIFI配网信息重置
* 实现WIFI远程通话和远程视频画面查看。
* 网络通讯架构：ESP32收到远端的mqtt请求{"cmd":"switch"}时，esp32启动Websocket服务连接， 向远端推送音频和视频数据。esp32可同步接收websocket发来的通话音频数据，通过扬声器播放。当再次收到mqtt请求时，关闭Websocket连接。
