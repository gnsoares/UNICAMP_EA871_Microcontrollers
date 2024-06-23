/**
 * @file I2C_OLED.h
 * @brief Prototypes, macros defined to the OLED display
 * @date 22/06/2023
 * @author Wu Shin-Ting
 * @note https://github.com/aleksey-makarov/stm32/blob/master/liboled/oled.c
 * @note https://www.instructables.com/Getting-Started-With-OLED-Displays/
 */
#ifndef I2C_OLED_H_
#define I2C_OLED_H_

#include <stdint.h>

#include "I2C.h"

// Expected sequence of data from the server SSD: control byte + data byte.
#define SSD1306_I2C 0b00111100  // 0x3C
// #define SSD1306_I2C 0b00111101 //0x3D

// Control byte.
// This is defined by Co (continuation bit) and D/C# (data/command selection bit), followed by six �0s.�
// Co = 0 (continuation) / 1 (single)
// D/C# = 0 (command) / 1 (data)
// Data Stream = 0b0100 0000 = 0x40
// Single Data Byte = 0b1100 0000 = 0xC0
// Single Command Byte = 0b1000 0000 = 0x80
// Command Stream = 0b0000 0000 = 0x00
typedef enum control_byte_tag {
    SSD1306_COMMAND = 0x80,
    SSD1306_DATA = 0xC0,
    SSD1306_DATA_CONTINUE = 0x40,
    SSD1306_COMMAND_CONTINUE = 0x00,
} control_byte;

// SSD1306 Commandset
// https://www.digikey.com/htmldatasheets/production/2047793/0/0/1/ssd1306.htm (Section 9)
// ------------------
// Fundamental Commands
#define SSD1306_SET_CONTRAST_CONTROL 0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME 0xA4
#define SSD1306_DISPLAY_ALL_ON 0xA5
#define SSD1306_NORMAL_DISPLAY 0xA6
#define SSD1306_INVERT_DISPLAY 0xA7
#define SSD1306_DISPLAY_OFF 0xAE
#define SSD1306_DISPLAY_ON 0xAF
#define SSD1306_NOP 0xE3
// Scrolling Commands
#define SSD1306_HORIZONTAL_SCROLL_RIGHT 0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT 0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT 0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT 0x2A
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
// Addressing Setting Commands
#define SSD1306_SET_LOWER_COLUMN 0x00
#define SSD1306_SET_HIGHER_COLUMN 0x10
#define SSD1306_MEMORY_ADDR_MODE 0x20
#define SSD1306_SET_COLUMN_ADDR 0x21
#define SSD1306_SET_PAGE_ADDR 0x22
// Hardware Configuration Commands
#define SSD1306_SET_START_LINE 0x40
#define SSD1306_SET_SEGMENT_REMAP 0xA0
#define SSD1306_SET_MULTIPLEX_RATIO 0xA8
#define SSD1306_COM_SCAN_DIR_INC 0xC0
#define SSD1306_COM_SCAN_DIR_DEC 0xC8
#define SSD1306_SET_DISPLAY_OFFSET 0xD3
#define SSD1306_SET_COM_PINS 0xDA
#define SSD1306_CHARGE_PUMP 0x8D
// Timing & Driving Scheme Setting Commands
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO 0xD5
#define SSD1306_SET_PRECHARGE_PERIOD 0xD9
#define SSD1306_SET_VCOM_DESELECT 0xDB

/**
 * @brief Initialize I2C connection to OLED via PORTB0 and PORTB1
 */
void I2C_initConSSD1306(void);
/**
 * @brief Initialize OLED display according to the commands described in
 * https://www.instructables.com/Getting-Started-With-OLED-Displays/
 */
void I2C_initOLED(void);
/**
 * @brief Write a byte to OLED
 */
uint8_t I2C_writeOLED_Byte(control_byte c, uint8_t data);
/**
 * @brief Write multiple bytes to OLED
 */
uint8_t I2C_writeOLED(control_byte c, uint32_t n_data, uint8_t *data);
/**
 * @brief Clear screen buffer
 */
void I2C_OLED_clrScrBuf();
/**
 * @brief Redisplay screen buffer
 */
void I2C_OLED_redisplay(void);
/**
 * @brief Set a pixel (x,y)
 * @param[in] x coordinate
 * @param[in] y coordinate
 */
void I2C_OLED_setPixel(uint16_t x, uint16_t y);
/**
 * @brief Reset a pixel (x,y)
 * @param[in] x coordinate
 * @param[in] y coordinate
 */
void I2C_OLED_clrPixel(uint16_t x, uint16_t y);
void I2C_OLED_write_A(uint16_t x, uint16_t y);
void I2C_OLED_write_E(uint16_t x, uint16_t y);
void I2C_OLED_write_I(uint16_t x, uint16_t y);
void I2C_OLED_write_N(uint16_t x, uint16_t y);
void I2C_OLED_write_P(uint16_t x, uint16_t y);
void I2C_OLED_write_Q(uint16_t x, uint16_t y);
void I2C_OLED_write_R(uint16_t x, uint16_t y);
void I2C_OLED_write_S(uint16_t x, uint16_t y);
void I2C_OLED_write_W(uint16_t x, uint16_t y);
void I2C_OLED_write_2(uint16_t x, uint16_t y);

#endif /* I2C_OLED_H_ */
