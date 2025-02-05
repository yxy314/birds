#ifndef __APP_MUSIC_H
#define __APP_MUSIC_H

#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ff.h"
#include "wavplay.h"
#include "exfuns.h"
#include "i2s.h"
#include "lcd.h"
#include "text.h"

void vMusicPlayTask(void *P);


/* 音乐播放控制器 */
typedef struct
{
    uint8_t *tbuf; /* 临时数组,仅在24bit解码的时候需要用到 */
    FIL *file;     /* 音频文件指针 */

    uint8_t status; /* bit0:0,暂停播放;1,继续播放 */
                    /* bit1:0,结束播放;1,开启播放 */
} __audiodev;

extern __audiodev g_audiodev; /* 音乐播放控制器 */

/******************************************************************************************/

void wav_sai_dma_callback(void);
void audio_start(void);                                                  /* 开始音频播放 */
void audio_stop(void);                                                   /* 停止音频播放 */
uint16_t audio_get_tnum(uint8_t *path);                                  /* 得到path路径下,目标文件的总个数 */
void audio_index_show(uint16_t index, uint16_t total);                   /* 显示曲目索引 */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate); /* 显示播放时间,比特率 信息 */
void audio_play(void);                                                   /* 播放音乐 */
uint8_t audio_play_song(uint8_t *fname);                                 /* 播放某个音频文件 */

extern TaskHandle_t xMusicPlayTaskHandle;
#endif
