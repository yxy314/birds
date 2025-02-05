#include "applicationmusic.h"
TaskHandle_t xMusicPlayTaskHandle = NULL;
__audiodev g_audiodev; /* 音乐播放控制器 */
void vMusicPlayTask(void *P)
{
    while (1)
    {
        /* 在这里只要没有通知音乐播放和暂停都不会执行，就阻塞在等待消息通知的位置
         * 即便收到了通知也只会执行一次
         * 可能会出现音乐响一声就停止的问题，因为只有在接收到通知的时候才会执行一次
         */
        BaseType_t notifiedValue = 0;
        xTaskNotifyWait(0, 0, &notifiedValue, 0); // 获取通知值
        /* 使用这个函数只能对通知值减一或清零操作 */
        // notifiedValue = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        if (notifiedValue) /* 阻塞等待，只有接收到通知才会开始判断和执行 */
        {
            LED(0);
            ESP_LOGI("MUSIC", "音乐播放");
                }
        else /* 音乐暂停 */
        {
            LED(1);
            audio_play();
            ESP_LOGI("MUSIC", "音乐暂停");
        }
        vTaskDelay(1000);
    }
}

/**
 * @brief       开始音频播放
 * @param       无
 * @retval      无
 */
void audio_start(void)
{
    g_audiodev.status = 3 << 0; /* 开始播放+非暂停 */
    i2s_trx_start();
}

/**
 * @brief       停止音频播放
 * @param       无
 * @retval      无
 */
void audio_stop(void)
{
    g_audiodev.status = 0;
    i2s_trx_stop();
}

/**
 * @brief       得到path路径下，目标文件的总数
 * @param       path : 文件路径
 * @retval      有效文件总数
 */
uint16_t audio_get_tnum(uint8_t *path)
{
    uint8_t res;
    uint16_t rval = 0;
    FF_DIR tdir;        /* 临时目录 */
    FILINFO *tfileinfo; /* 临时文件信息 */

    tfileinfo = (FILINFO *)malloc(sizeof(FILINFO)); /* 申请内存 */

    res = f_opendir(&tdir, (const TCHAR *)path); /* 打开目录 */

    if ((res == FR_OK) && tfileinfo)
    {
        while (1) /* 查询总的有效文件数 */
        {
            res = f_readdir(&tdir, tfileinfo); /* 读取目录下的一个文件 */

            if ((res != FR_OK) || (tfileinfo->fname[0] == 0))
            {
                break; /* 错误了/到末尾了,退出 */
            }

            res = exfuns_file_type(tfileinfo->fname);

            if ((res & 0xF0) == 0x40) /* 取高四位,看看是不是音乐文件 */
            {
                rval++; /* 有效文件数增加1 */
            }
        }
    }

    free(tfileinfo); /* 释放内存 */

    return rval;
}

/**
 * @brief       显示曲目索引
 * @param       index : 当前索引
 * @param       total : 总文件数
 * @retval      无
 */
void audio_index_show(uint16_t index, uint16_t total)
{
    /* 显示当前曲目的索引,及总曲目数 */
    lcd_show_num(30 + 0, 230, index, 3, 16, RED); /* 索引 */
    lcd_show_char(30 + 24, 230, '/', 16, 0, RED);
    lcd_show_num(30 + 32, 230, total, 3, 16, RED); /* 总曲目 */
}

/**
 * @brief       显示播放时间,比特率 信息
 * @param       totsec : 音频文件总时间长度
 * @param       cursec : 当前播放时间
 * @param       bitrate: 比特率(位速)
 * @retval      无
 */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate)
{
    static uint16_t playtime = 0xFFFF; /* 放时间标记 */

    if (playtime != cursec) /* 需要更新显示时间 */
    {
        playtime = cursec;

        /* 显示播放时间 */
        lcd_show_xnum(30, 210, playtime / 60, 2, 16, 0X80, RED); /* 分钟 */
        lcd_show_char(30 + 16, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 24, 210, playtime % 60, 2, 16, 0X80, RED); /* 秒钟 */
        lcd_show_char(30 + 40, 210, '/', 16, 0, RED);

        /* 显示总时间 */
        lcd_show_xnum(30 + 48, 210, totsec / 60, 2, 16, 0X80, RED); /* 分钟 */
        lcd_show_char(30 + 64, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 72, 210, totsec % 60, 2, 16, 0X80, RED); /* 秒钟 */

        /* 显示位率 */
        lcd_show_num(30 + 110, 210, bitrate / 1000, 4, 16, RED); /* 显示位率 */
        lcd_show_string(30 + 110 + 32, 210, 200, 16, 16, "Kbps", RED);
    }
}

/**
 * @brief       播放音乐
 * @param       无
 * @retval      无
 */
void audio_play(void)
{
    uint8_t res;
    FF_DIR wavdir;        /* 目录 */
    FILINFO *wavfileinfo; /* 文件信息 */
    uint8_t *pname;       /* 带路径的文件名 */
    uint16_t totwavnum;   /* 音乐文件总数 */
    uint16_t curindex;    /* 当前索引 */
    uint8_t key;          /* 键值 */
    uint32_t temp;
    uint32_t *wavoffsettbl; /* 音乐offset索引表 */

    es8388_adda_cfg(1, 0);   /* 开启DAC关闭ADC */
    es8388_output_cfg(1, 1); /* DAC选择通道1输出 */

    while (f_opendir(&wavdir, "0:/MUSIC")) /* 打开音乐文件夹 */
    {
        text_show_string(30, 190, 240, 16, "MUSIC文件夹错误!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 206, WHITE); /* 清除显示 */
        vTaskDelay(200);
    }

    totwavnum = audio_get_tnum((uint8_t *)"0:/MUSIC"); /* 得到总有效文件数 */

    while (totwavnum == NULL) /* 音乐文件总数为0 */
    {
        text_show_string(30, 190, 240, 16, "没有音乐文件!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 146, WHITE); /* 清除显示 */
        vTaskDelay(200);
    }

    wavfileinfo = (FILINFO *)malloc(sizeof(FILINFO)); /* 申请内存 */
    pname = malloc(255 * 2 + 1);                      /* 为带路径的文件名分配内存 */
    wavoffsettbl = malloc(4 * totwavnum);             /* 申请4*totwavnum个字节的内存,用于存放音乐文件off block索引 */

    while (!wavfileinfo || !pname || !wavoffsettbl) /* 内存分配出错 */
    {
        text_show_string(30, 190, 240, 16, "内存分配失败!", 16, 0, BLUE);
        vTaskDelay(200);
        lcd_fill(30, 190, 240, 146, WHITE); /* 清除显示 */
        vTaskDelay(200);
    }

    /* 记录索引 */
    res = f_opendir(&wavdir, "0:/MUSIC"); /* 打开目录 */

    if (res == FR_OK)
    {
        curindex = 0; /* 当前索引为0 */

        while (1) /* 全部查询一遍 */
        {
            temp = wavdir.dptr; /* 记录当前index */

            res = f_readdir(&wavdir, wavfileinfo); /* 读取目录下的一个文件 */

            if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
            {
                break; /* 错误了/到末尾了,退出 */
            }

            res = exfuns_file_type(wavfileinfo->fname);

            if ((res & 0xF0) == 0x40) /* 取高四位,看看是不是音乐文件 */
            {
                wavoffsettbl[curindex] = temp; /* 记录索引 */
                curindex++;
            }
        }
    }

    curindex = 0;                                        /* 从0开始显示 */
    res = f_opendir(&wavdir, (const TCHAR *)"0:/MUSIC"); /* 打开目录 */

    while (res == FR_OK) /* 打开成功 */
    {
        dir_sdi(&wavdir, wavoffsettbl[curindex]); /* 改变当前目录索引 */
        res = f_readdir(&wavdir, wavfileinfo);    /* 读取目录下的一个文件 */

        if ((res != FR_OK) || (wavfileinfo->fname[0] == 0))
        {
            break; /* 错误了/到末尾了,退出 */
        }

        strcpy((char *)pname, "0:/MUSIC/");                      /* 复制路径(目录) */
        strcat((char *)pname, (const char *)wavfileinfo->fname); /* 将文件名接在后面 */
        lcd_fill(30, 190, lcd_self.width - 1, 190 + 16, WHITE);  /* 清除之前的显示 */
        audio_index_show(curindex + 1, totwavnum);
        text_show_string(30, 190, lcd_self.width - 60, 16, (char *)wavfileinfo->fname, 16, 0, BLUE); /* 显示歌曲名字 */
        key = audio_play_song(pname);                                                                /* 播放这个音频文件 */

        if (key == KEY2_PRES) /* 上一曲 */
        {
            if (curindex)
            {
                curindex--;
            }
            else
            {
                curindex = totwavnum - 1;
            }
        }
        else if (key == KEY0_PRES) /* 下一曲 */
        {
            curindex++;

            if (curindex >= totwavnum)
            {
                curindex = 0; /* 到末尾的时候,自动从头开始 */
            }
        }
        else
        {
            break; /* 产生了错误 */
        }
    }

    free(wavfileinfo);  /* 释放内存 */
    free(pname);        /* 释放内存 */
    free(wavoffsettbl); /* 释放内存 */
}

/**
 * @brief       播放某个音频文件
 * @param       fname : 文件名
 * @retval      按键值
 *   @arg       KEY0_PRES , 下一曲.
 *   @arg       KEY2_PRES , 上一曲.
 *   @arg       其他 , 错误
 */
uint8_t audio_play_song(uint8_t *fname)
{
    uint8_t res;

    res = exfuns_file_type((char *)fname);

    switch (res)
    {
    case T_WAV:
        res = wav_play_song(fname);
        break;
    case T_MP3:
        /* 自行实现 */
        break;

    default: /* 其他文件,自动跳转到下一曲 */
        printf("can't play:%s\r\n", fname);
        res = KEY0_PRES;
        break;
    }
    return res;
}
