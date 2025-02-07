#include "HardwareManager.h"
i2c_obj_t i2c0_master;
void vSystemHardwareDriverInit(void)
{
    led_init();                        /* 初始化LED */
    key_init();                        /* 初始化按键 */
    i2c0_master = iic_init(I2C_NUM_0); /* 初始化IIC0 */
    spi2_init();                       /* 初始化SPI2 */
    xl9555_init(i2c0_master);          /* IO扩展芯片初始化 */
    lcd_init();
    nvs_init();
    es8388_init(i2c0_master); /* ES8388初始化 */
    es8388_adda_cfg(1, 0);    /* 开启DAC关闭ADC */
    es8388_input_cfg(0);
    es8388_output_cfg(1, 1);        /* DAC选择通道输出 */
    es8388_hpvol_set(0);           /* 设置耳机音量 */
    es8388_spkvol_set(0);          /* 设置喇叭音量 */
    xl9555_pin_write(SPK_EN_IO, 0); /* 打开喇叭 */
    i2s_init();                     /* I2S初始化 */
}
