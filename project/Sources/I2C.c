/**
 * @file I2C.c
 * @brief Basic functions for I2C module
 * @date 22/06/2023
 * @author Wu Shin-Ting
 * @note https://embarcados.com.br/biblioteca-i2c-para-frdm-kl25z/
 *
 */

#include "I2C.h"

static I2C_MemMapPtr I2C[] = I2C_BASE_PTRS;

/****************************************************************************************
 *
 *****************************************************************************************/
uint8_t I2C_Init(uint8_t x, uint8_t alt, uint8_t mult, uint8_t icr) {
    if (x == 0) {
        switch (alt) {
            case ALT0:
                SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;  // Turn on clock to E module
                PORTE_PCR24 |= PORT_PCR_MUX(0x5);   // Set PTE24 to mux 5 [I2C_SCL]
                PORTE_PCR25 |= PORT_PCR_MUX(0x5);   // Set PTE25 to mux 5 [I2C_SDA]
                break;

            case ALT1:
                SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;  // Turn on clock to E module
                PORTB_PCR0 |= PORT_PCR_MUX(0x2);    // Set PTB0 to mux 2 [I2C_SCL]
                PORTB_PCR1 |= PORT_PCR_MUX(0x2);    // Set PTB1 to mux 2 [I2C_SDA]
                break;

            case ALT2:
                SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;  // Turn on clock to E module
                PORTB_PCR2 |= PORT_PCR_MUX(0x2);    // Set PTB2 to mux 2 [I2C_SCL]
                PORTB_PCR3 |= PORT_PCR_MUX(0x2);    // Set PTB3 to mux 2 [I2C_SDA]
                break;

            case ALT3:
                SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // Turn on clock to E module
                PORTC_PCR8 |= PORT_PCR_MUX(0x2);    // Set PTB2 to mux 2 [I2C_SCL]
                PORTC_PCR9 |= PORT_PCR_MUX(0x2);    // Set PTB3 to mux 2 [I2C_SDA]
                break;

            default:
                return 0;
                break;
        }
        SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
    } else if (x == 1) {
        switch (alt) {
            case ALT0:
                SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;  // Turn on clock to E module
                PORTE_PCR0 |= PORT_PCR_MUX(0x6);    // Set PTE0 to mux 6 [I2C_SDA]
                PORTE_PCR1 |= PORT_PCR_MUX(0x6);    // Set PTE1 to mux 6 [I2C_SCL]
                break;

            case ALT1:
                SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;  // Turn on clock to A module
                PORTA_PCR3 |= PORT_PCR_MUX(0x2);    // Set PTA3 to mux 2 [I2C_SCL]
                PORTA_PCR4 |= PORT_PCR_MUX(0x2);    // Set PTA4 to mux 2 [I2C_SDA]
                break;

            case ALT2:
                SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // Turn on clock to C module
                PORTC_PCR1 |= PORT_PCR_MUX(0x2);    // Set PTC1 to mux 2 [I2C_SCL]
                PORTC_PCR2 |= PORT_PCR_MUX(0x2);    // Set PTC2 to mux 2 [I2C_SDA]
                break;

            case ALT3:
                SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // Turn on clock to E module
                PORTC_PCR10 |= PORT_PCR_MUX(0x2);   // Set PTC10 to mux 2 [I2C_SCL]
                PORTC_PCR11 |= PORT_PCR_MUX(0x2);   // Set PTC11 to mux 2 [I2C_SDA]
                break;

            default:
                return 0;
                break;
        }
        SIM_SCGC4 |= SIM_SCGC4_I2C1_MASK;
    } else {
        return 0;
    }
    I2C[x]->F = I2C_F_ICR(icr) | I2C_F_MULT(mult);

    I2C[x]->C1 = (I2C_C1_IICEN_MASK |  // I2C Enable
                  I2C_C1_IICIE_MASK);  // I2C Interrupt Enable

    return 1;
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_Start(uint8_t x) {
    I2C[x]->C1 |= I2C_C1_TX_MASK;
    I2C[x]->C1 |= I2C_C1_MST_MASK;
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_Stop(uint8_t x) {
    I2C[x]->C1 &= ~I2C_C1_MST_MASK;
    I2C[x]->C1 &= ~I2C_C1_TX_MASK;
}
/****************************************************************************************
 *
 *****************************************************************************************/
uint8_t I2C_Wait(uint8_t x) {
    uint32_t i = 1000000;

    while ((!(I2C[x]->S & I2C_S_IICIF_MASK)) && i) {
        i--;
    }

    I2C[x]->S |= I2C_S_IICIF_MASK;  // w1c

    if (i == 0) return 0;
    return 1;
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_WaitStop(uint8_t x) {
    /*
     * It is necessary to wait stop actually taking place and completing
     * https://community.nxp.com/t5/Kinetis-Microcontrollers/Why-is-there-a-pause-in-I2C-routines/td-p/245449
     */
    while (I2C[x]->S & I2C_S_BUSY_MASK);
}

/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_WriteByte(uint8_t x, uint8_t data) {
    I2C[x]->D = (uint8_t)data;
}
/****************************************************************************************
 *
 *****************************************************************************************/
void I2C_WriteMultData(uint8_t x, uint8_t SlaveAddress,
                       uint32_t n_data, uint8_t *data) {
    uint32_t i = 0;

    I2C_Start(x);
    I2C_WriteByte(x, ((SlaveAddress << 1) | I2C_WRITE));
    I2C_Wait(x);

    for (i = 0; i < n_data; i++) {
        I2C_WriteByte(x, *data);
        I2C_Wait(x);
        data++;
    }
    I2C_Stop(x);

    I2C_WaitStop(0);
}
