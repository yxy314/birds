#include "applicationled.h"

/*
 * Filename   ：applicationled.c
 * Author     : zdd
 * Version    : 1.0
 * Date       : 2024.12.20
 * Discription : LED处理应用函数
 */

/*******************************************************************************
 * Function Name  : vLedStateDisplay
 * Description    : LED显示
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void vLedStateDisplay(void)
{
    LED_TOGGLE(); /* 调用一次引脚电平反转一次，用于测试阿里云服务器数据收发状态 */
}
