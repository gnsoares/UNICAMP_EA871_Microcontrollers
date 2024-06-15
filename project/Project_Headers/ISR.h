/**
 * @file ISR.h
 * @brief Este modulo contem declaracoes relacionadas com o modulo ISR.c
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @date 15/02/2024
 */

#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>

typedef enum {
    PREPARA_INICIO,
    INICIO,
    LAUNCH_BALL,
    PLAYER_TURN,
    LCD_UPDATE,
    WIN_SCREEN,
    WIN_VISU
} state_t;

/**
 * @brief Muda o estado do programa
 *
 * @param[in] s novo estado
 */
void ISR_setState(state_t s);

/**
 * @brief Retorna estado atual do programa
 *
 * @return estado atual
 */
state_t ISR_getState(void);

#endif /* ISR_H_ */
