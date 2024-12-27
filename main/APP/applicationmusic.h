#ifndef __APP_MUSIC_H
#define __APP_MUSIC_H

#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void vMusicPlayTask(void *P);

extern TaskHandle_t xMusicPlayTaskHandle;
#endif
