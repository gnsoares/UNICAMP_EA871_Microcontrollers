#ifndef _MCU_H
#define _MCU_H

#include "GPIO_latch_lcd.h"
#include "GPIO_switches.h"
#include "RTC.h"
#include "TPM.h"

void config(void);
float get_time(void);

#endif
