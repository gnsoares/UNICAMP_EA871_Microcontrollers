/**
 * @file I2C.h
 * @brief Prototypes and macros defined to the I2C module
 * @date 22/06/2023
 * @author Wu Shin-Ting
 * @note https://embarcados.com.br/biblioteca-i2c-para-frdm-kl25z/
 */

#ifndef I2C_H_
#define I2C_H_

#include "MKL25Z4.h"
#include "util.h"

#define ALT0 0
#define ALT1 1
#define ALT2 2
#define ALT3 3
#define MULT0 0
#define MULT1 1
#define MULT2 2

#define I2C_WRITE 0  // Master write
#define I2C_READ 1   // Master read

/**
 * @brief initializes a I2C connection
 * @param[in] x I2Cx module
 * @param[in] alt ALT0, ALT1, ALT2 and ALT3 pins for each module
 * @param[in] mult multiplier factor for SCL divider
 * @param[in] icr SCL clock rate
 */
uint8_t I2C_Init(uint8_t x, uint8_t alt, uint8_t mult, uint8_t icr);
/**
 * @brief sets START condition (changes to Transmit and Master modes)
 * @param[in] x I2Cx module
 */
void I2C_Start(uint8_t x);
/**
 * @brief relinquishes the bus (changes to Receiver and Slave modes)
 * @param[in] x I2Cx module
 */
void I2C_Stop(uint8_t x);

/**
 * @brief waits for completion of a byte and acknowledge bit transfer
 * @param[in] x I2Cx module
 */
uint8_t I2C_Wait(uint8_t x);
/**
 * @brief waits stop actually taking place and completing
 * @note https://community.nxp.com/t5/Kinetis-Microcontrollers/Why-is-there-a-pause-in-I2C-routines/td-p/24544
 * @param[in] x I2Cx module
 */
void I2C_WaitStop(uint8_t x);
/**
 * @brief sends one byte
 * @param[in] x I2Cx module
 * @param[in] data byte to be sent
 */
void I2C_WriteByte(uint8_t x, uint8_t data);
/**
 * @brief writes multiple bytes to a slave
 * @param[in] x I2Cx module
 * @param[in] SlaveAddress
 * @param[in] n_data number of bytes
 * @param[in] *data data vector address
 */
void I2C_WriteMultData(uint8_t x, uint8_t SlaveAddress, uint32_t n_data, uint8_t *data);

#endif /* I2C_H_ */
