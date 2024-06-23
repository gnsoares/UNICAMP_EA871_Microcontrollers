/**
 * @file mcu.h
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief Prototipos, macros e tipos de dados de mcu
 * @date 2024-06-15
 */

#ifndef _MCU_H
#define _MCU_H

#include <MKL25Z4.h>

#include "GPIO_lcd.h"
#include "GPIO_switches.h"
#include "I2C_OLED.h"
#include "OSC.h"
#include "RTC.h"
#include "SIM.h"
#include "TPM.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BTN_IRQC 0b1010  // falling edge

/**
 * @brief Configuracao basica do microncontrolador e perifericos
 *
 */
void config(void);
/**
 * @brief Reinicia o RTC em zero
 *
 */
void reset_time(void);
/**
 * @brief Obtem o tempo medido pelo modulo RTC
 *
 * @return tempo em milissegundos
 */
uint32_t get_time(void);

#endif
