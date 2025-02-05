#ifndef __HARDWAREMANAGE_H
#define __HARDWAREMANAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "nvs.h"
#include "es8388.h"
#include "xl9555.h"
#include "i2s.h"
#include "spi.h"
#include "iic.h"
void vSystemHardwareDriverInit(void);

#endif