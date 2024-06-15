/**
 * @file ISR.c
 * @brief Definicao das rotinas de servico
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @date 04/05/2024
 */

// Inclusao de macros do IDE
#include "derivative.h"

// Inclusao de prototipos
#include "ISR.h"
#include "TPM.h"
#include "util.h"

static state_t state;

void FTM1_IRQHandler() {
    static uint16_t tempo = 0;
    static uint16_t i = 0;
    uint16_t valor;

    // TOF
    if (TPM1_SC & TPM_SC_TOIE_MASK && TPM1_STATUS & (TPM_STATUS_TOF_MASK)) {
        if (tempo == 0) {
            valor = (uint16_t)((pacman[i] * 20971520) / 128);  // seta nova nota

            TPM_setaMOD(1, valor);
            TPM_setaCnV(1, 1, (uint16_t)(valor * 0.5));  // amplitude: 1/2 potencia

            tempo++;
        } else if (tempo >= 0x29) {
            tempo = 0;
            i++;
            if (i == 32) i = 0;  // repete a sequencia
        } else {
            tempo++;  // ~1.6s
        }
        TPM1_SC |= TPM_SC_TOF_MASK;
    }
    // borda de descida de PTA12
    else if (TPM1_C0SC & TPM_CnSC_CHIE_MASK && TPM1_C0SC & TPM_CnSC_CHF_MASK) {
        if (state == INICIO) {
            TPM2_C0V = TPM2_CNT;  // referencia e o valor atual do contador
            TPM_habilitaInterrupCH(2, 0);
            TPM_desabilitaInterrupCH(1, 0);  // desabilita interrupcao do botao
            state = PREPARA_AUDITIVO;
        }
        TPM1_C0SC |= TPM_CnSC_CHF_MASK;
    }
}

void ISR_setState(state_t s) {
    state = s;
}

state_t ISR_getState(void) {
    return state;
}
