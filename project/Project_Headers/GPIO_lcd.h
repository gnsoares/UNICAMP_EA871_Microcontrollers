/**
 * @file GPIO_lcd.h
 * @brief Prototipos, macros e tipos de dados de GPIO_lcd
 * @author Wu Shin Ting
 * @date 02/02/2024
 */

#ifndef GPIO_LCD_H_
#define GPIO_LCD_H_

#include <stdint.h>

/**
 * @brief Funcao do byte enviado para LCD
 */
typedef enum lcd_RS_tag {
    COMANDO,  //!< Comando (0)
    DADO      //!< Dado (1)
} tipo_lcd_RS;

/*!
 * @brief Habilita conexao entre mcu e LCD.
 */
void GPIO_LCD_ativa_con();

/*!
 * @brief Seta o tipo de informacao enviada para LCD.
 * @param[in] i (COMANDO = 0; DADO = 1)
 */
void GPIO_LCD_set_RS(tipo_lcd_RS i);

/*!
 * @brief Transfere um byte para LCD.
 * @param[in] c byte
 * @param[in] t tempo de processamento em us
 */
void GPIO_LCD_escreve_byte(uint8_t c, uint16_t t);

/*!
 * @brief Inicializa LCD
 */
void GPIO_LCD_init();

/*!
 * @brief Escreve uma string de caracteres a partir de um endereco
 * @param[in] end endereco DDRAM
 * @param[in] str endereco inicial da string
 */
void GPIO_LCD_escreve_string(uint8_t end, uint8_t *str);

#endif /* GPIO_LATCH_LCD_H_ */
