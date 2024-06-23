/**
 * @file I2C_OLED.c
 * @brief Functions related to the OLED display with controller SSD1306
 * @date 22/06/2023
 * @author Wu Shin-Ting
 */

#include "I2C_OLED.h"

#include "I2C.h"
#include "string.h"

uint8_t init_cmds[] = {
    SSD1306_DISPLAY_OFF,
    SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO,
    0x80,
    SSD1306_SET_MULTIPLEX_RATIO,
    0x3F,
    SSD1306_SET_DISPLAY_OFFSET,
    0x0,
    SSD1306_SET_START_LINE | 0x0,
    SSD1306_CHARGE_PUMP,
    0x14,
    SSD1306_MEMORY_ADDR_MODE,
    0x00,
    SSD1306_SET_SEGMENT_REMAP | 0x1,
    SSD1306_COM_SCAN_DIR_DEC,
    SSD1306_SET_COM_PINS,
    0x12,
    SSD1306_SET_CONTRAST_CONTROL,
    0xCF,
    SSD1306_SET_PRECHARGE_PERIOD,
    0xF1,
    SSD1306_SET_VCOM_DESELECT,
    0x40,
    SSD1306_DISPLAY_ALL_ON_RESUME,
    SSD1306_NORMAL_DISPLAY,
    SSD1306_DISPLAY_ON,
};

uint8_t update_cmds[] = {
    SSD1306_SET_COLUMN_ADDR,
    0,
    127,
    SSD1306_SET_PAGE_ADDR,
    0,
    7,
};

#define SCRBUF_SIZE 1024
#define SCRBUF_CMD_SIZE (SCRBUF_SIZE + 1)
static uint8_t scrbuf_cmd[SCRBUF_CMD_SIZE] = {
    SSD1306_DATA_CONTINUE,
};
static uint8_t *const scrbuf = scrbuf_cmd + 1;

void I2C_initConSSD1306(void) {
    /*
     * Initialize module I2C2 for connection
     * Default bus clock frequency is 20971520 (47,68ns)
     * SSD1306 controller (display OLED) specifications
     * (https://datasheethub.com/wp-content/uploads/2022/08/SSD1306.pdf)
     * baud rate < 400000Hz; Typical value = 115200Hz; Used: 100kHz
     * mul = 1
     * 115200 = 20971520/(mul*SCL divider) -> SCL divider = 20971520/(100000*1) = 209,7152
     * SDA hold time = tHD = 300ns -> SDA hold value = 300/(47,68*1) = 6,292
     * SCL start hold time = tHSTART = 0.6us -> SCL start hold value = 600/(47,68*1) = 12,58
     * SCL stop hold time = tSSTOP = 0.6us -> SCL stop hold value = 600/(47,68*1) = 12,58
     * ICR = 0x22
     */
    I2C_Init(0, ALT1, MULT0, 0x22);
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_clrScrBuf() {
    memset(scrbuf, 0, SCRBUF_SIZE);
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_redisplay(void) {
    uint8_t *tmp;
    uint8_t v[2] = {SSD1306_COMMAND_CONTINUE, 0x00};
    uint32_t i;

    tmp = update_cmds;
    for (i = sizeof(update_cmds); i; i--, tmp++) {
        v[1] = *tmp;
        I2C_WriteMultData(0, SSD1306_I2C, 2, v);
    }

    I2C_WriteMultData(0, SSD1306_I2C, SCRBUF_CMD_SIZE, scrbuf_cmd);
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_initOLED(void) {
    uint8_t *tmp;
    uint8_t v[2] = {SSD1306_COMMAND_CONTINUE, 0x00};
    uint32_t i;

    // List of commands for reset: Section 8.5 in
    // https://www.digikey.com/htmldatasheets/production/2047793/0/0/1/ssd1306.html

    tmp = init_cmds;

    for (i = sizeof(init_cmds); i; i--, tmp++) {
        v[1] = *tmp;
        I2C_WriteMultData(0, SSD1306_I2C, 2, v);
    }

    // Fill the screenbuffer
    I2C_OLED_clrScrBuf();

    I2C_OLED_redisplay();
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_setPixel(uint16_t x, uint16_t y) {
    int by, bi;

    if (x < 128 && y < 64) {
        by = ((y / 8) * 128) + x;
        bi = y % 8;

        scrbuf[by] |= (1 << bi);
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_clrPixel(uint16_t x, uint16_t y) {
    int by, bi;

    if (x < 128 && y < 64) {
        by = ((y / 8) * 128) + x;
        bi = y % 8;

        scrbuf[by] &= ~(1 << bi);
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_A(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if ((j == y || j == y + 5) ||                           // top and middle lines
                (i == x || i == x + 1 || i == x + 6 || i == x + 7)  // left and right lines
            ) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_E(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if (i == x || i == x + 1 ||
                (i > x && i < x + 4 && j == y) ||
                (j == y + 4 && i < x + 4) ||
                (i > x && i < x + 4 && j == y + 7)) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_I(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if (i == x + 3 || i == x + 4) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_N(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if (i == x || i == x + 7 || i - j == x - y) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_P(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if (i == x || i == x + 1 ||
                (i > x && i < x + 6 && j == y) ||
                (i == x + 5 && j < y + 4) ||
                (j == y + 4 && i < x + 4)) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_Q(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if ((i == x || i == x + 1 || i == x + 6 || i == x + 7) ||
                ((j == y || j == y + 7) && (i > x && i < x + 8)) ||
                (i > x + 3 && i < x + 8 && j > y + 3) ||
                (i == x + 3 && j == y + 5)) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_R(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if (i == x || i == x + 1 ||
                (i > x && i < x + 4 && j == y) ||
                (i == x + 3 && j < y + 4) ||
                (j == y + 4 && i < x + 4) ||
                (i > x + 1 && i == x + j - y)) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_S(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if ((i > x && i < x + 6 && (j == y || j == y + 7)) ||
                (j == y + 3 && i > x && i < x + 6) ||
                ((i == x || i == x + 1) && (j < y + 4)) ||
                ((i == x + 5 || i == x + 4) && (j >= y + 4))) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_W(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if ((i == x || i == x + 7) ||
                (j == y + 7 && (i == x + 1 || i == x + 6)) ||
                ((j == y + 6 || j == y + 5) && (i == x + 2 || i == x + 5)) ||
                ((j == y + 4 || j == y + 3) && (i == x + 3 || i == x + 4))) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_OLED_write_2(uint16_t x, uint16_t y) {
    uint16_t i, j;
    for (i = x; i < x + 8; i++) {
        for (j = y; j < y + 8; j++) {
            if ((j == y || j == y + 7) ||  // top and bottom lines
                (i == x && j > y + 4) ||
                (i == x + 7 && j < y + 3) ||
                (j == y + 3 && i > x && i < x + 7)) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
}
