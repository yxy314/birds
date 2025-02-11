/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-26
 * @brief       WIFI Aliyun实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "lcd.h"
#include "wifi_config.h"
#include "lwip_demo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "key.h"
#include "HardwareManager.h"
#include "TaskManage.h"
#include "spi_sdcard.h"
/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t key = 0;
    vSystemHardwareDriverInit(); /* 硬件初始化 */

    /* 分割线 */
    while (sd_spi_init()) /* 检测不到SD卡 */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(500);
    }
    while (fonts_init()) /* 检查字库 */
    {
        lcd_clear(WHITE); /* 清屏 */
        lcd_show_string(30, 30, 200, 16, 16, "ESP32-S3", RED);

        key = fonts_update_font(30, 50, 16, (uint8_t *)"0:", RED); /* 更新字库 */

        while (key) /* 更新失败 */
        {
            lcd_show_string(30, 50, 200, 16, 16, "Font Update Failed!", RED);
            vTaskDelay(200);
            lcd_fill(20, 50, 200 + 20, 90 + 16, WHITE);
            vTaskDelay(200);
        }

        lcd_show_string(30, 50, 200, 16, 16, "Font Update Success!   ", RED);
        vTaskDelay(1500);
        lcd_clear(WHITE); /* 清屏 */
    }
    //vTaskCreateFunction(); /* 任务初始化创建 */
    wifi_sta_init();       /* 网络配置 */
    lwip_demo();           /* lwip测试代码 */
    lcd_show_string(0, 0, 240, 32, 32, "ESP32-S3", RED);
    lcd_show_string(0, 40, 240, 24, 24, "WiFi Aliyun Test", RED);
    lcd_show_string(0, 70, 240, 16, 16, "ATOM@ALIENTEK", RED);
    while (1)
    {
        audio_play();
    }
}
