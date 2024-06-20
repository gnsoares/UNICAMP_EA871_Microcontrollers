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

#include "game.h"

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

void ISR_setPlayer(player_t p);
player_t ISR_getPlayer(void);
void ISR_swapPlayer(void);
board_t* ISR_getBoard(void);

#endif /* ISR_H_ */
