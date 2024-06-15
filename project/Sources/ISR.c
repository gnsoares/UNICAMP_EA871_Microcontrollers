/**
 * @file ISR.c
 * @brief Definicao das rotinas de servico
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @date 04/05/2024
 */

#include "ISR.h"

#include "game.h"
#include "mcu.h"
#include "util.h"

static state_t state;
static player_t player = PLAYER_1;

// TODO: review this function
void FTM1_IRQHandler() {
    static uint16_t tempo = 0;
    uint16_t valor;

    // TOF
    if (TPM1_SC & TPM_SC_TOIE_MASK && TPM1_STATUS & (TPM_STATUS_TOF_MASK)) {
        if (tempo == 0) {
            valor = (uint16_t)((0.003405 * 20971520) / 128);  // seta nova nota
            TPM_setaMOD(1, valor);
            TPM_setaCnV(1, 1, (uint16_t)(valor * 0.5));  // amplitude: 1/2 potencia
        }
        if (tempo < 50) {
            tempo++;
        } else {
            TPM_setaMOD(1, 0);
            TPM_setaCnV(1, 1, 0);
            tempo = 0;
            TPM_desabilitaInterrupTOF(1);
        }
        TPM1_SC |= TPM_SC_TOF_MASK;
    }
}

void PORTA_IRQHandler() {
    if (PORTA_PCR4 & PORT_PCR_ISF_MASK) {
        if (state == PLAYER_TURN && player == PLAYER_1) {
            TPM_habilitaInterrupTOF(1);  // play hit sound
            // TODO: disable interrupt of button
            // TODO: check timing of press
            // TODO: change ball vx and vy accordingly
            ISR_swapPlayer();
            // TODO: enable interrupt of other player's button
        }
        PORTA_PCR4 |= PORT_PCR_ISF_MASK;  // w1c: limpa flag de interrupcao
    } else if (PORTA_PCR5 & PORT_PCR_ISF_MASK) {
        if (state == PLAYER_TURN && player == PLAYER_2) {
            TPM_habilitaInterrupTOF(1);  // play hit sound
            // TODO: disable interrupt of button
            // TODO: check timing of press
            // TODO: change ball vx and vy accordingly
            ISR_swapPlayer();
            // TODO: enable interrupt of other player's button
        }
        PORTA_PCR5 |= PORT_PCR_ISF_MASK;  // w1c: limpa flag de interrupcao
    } else if (PORTA_PCR12 & PORT_PCR_ISF_MASK) {
        if (state == INICIO) {
            // TODO: disable interrupt of button
            state = LAUNCH_BALL;
        }
        PORTA_PCR5 |= PORT_PCR_ISF_MASK;  // w1c: limpa flag de interrupcao
    }
}

void ISR_setState(state_t s) {
    state = s;
}

state_t ISR_getState(void) {
    return state;
}

player_t ISR_getPlayer(void) {
    return player;
}

void ISR_swapPlayer(void) {
    player = (player == PLAYER_1 ? PLAYER_2 : PLAYER_1);
}
