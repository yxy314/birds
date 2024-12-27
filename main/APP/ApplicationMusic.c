#include "applicationmusic.h"
TaskHandle_t xMusicPlayTaskHandle = NULL;
void vMusicPlayTask(void *P)
{
    uint16_t i = 0;
    while (1)
    {
        /* 在这里只要没有通知音乐播放和暂停都不会执行，就阻塞在等待消息通知的位置
         * 即便收到了通知也只会执行一次
         * 可能会出现音乐响一声就停止的问题，因为只有在接收到通知的时候才会执行一次
         */
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) /* 阻塞等待，只有接收到通知才会开始判断和执行 */
        {
            LED_TOGGLE();
            ESP_LOGI("MUSIC", "音乐播放%d", i++);
        }
        else /* 音乐暂停 */
        {
            LED_TOGGLE();
            ESP_LOGI("MUSIC", "音乐暂停%d", i++);
        }

        vTaskDelay(1000);
    }
}