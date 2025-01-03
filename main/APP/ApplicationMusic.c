#include "applicationmusic.h"
TaskHandle_t xMusicPlayTaskHandle = NULL;
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
        //notifiedValue = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        if (notifiedValue) /* 阻塞等待，只有接收到通知才会开始判断和执行 */
        {
            LED(0);
            ESP_LOGI("MUSIC", "音乐播放");
        }
        else /* 音乐暂停 */
        {
            LED(1);
            ESP_LOGI("MUSIC", "音乐暂停");
        }
        vTaskDelay(1000);
    }
}