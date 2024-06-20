/**
 * @file RTC.c
 * @brief Definicao dos prototipos relacionados com RTC
 * @date 08/02/2024
 * @author Wu Shin-Ting
 */

#include "mcu.h"
#include "util.h"

void RTClpo_init() {
    SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;  // ativar o sinal de relogio do RTC

    RTC_CR |= RTC_CR_SWR_MASK;   // resetar os registradores do RTC por software
                                 // limpar o contador RTC_TSR e flags RTC_SR_TOF e RTC_SR_TIF
    RTC_CR &= ~RTC_CR_SWR_MASK;  // resetar explicitamente SWR

    RTC_TCR = RTC_TCR_CIR(0x00) |  // ajustar o erro de frequencia (ppm) em cada segundo
              RTC_TCR_TCR(0x00);   // 1s = 32.768 ticks (sem ajuste)

    RTC_TPR = 0;  // resetar prescaler (2^15 - 1)
    RTC_TSR = 0;  // resetar o contador para que TIF seja baixada.

    RTC_SR |= RTC_SR_TCE_MASK;  // habilita o relogio
}

uint32_t RTClpo_getTime(uint32_t *seconds) {
    //	*seconds = (RTC_TSR*32768)/1000+(RTC_TPR/1000);  //!< reajustar a contagem para segundos
    *seconds = ((RTC_TSR * 32768) + RTC_TPR) / 1000;  //!< reajustar a contagem para segundos

    return *seconds;
}

void RTClpo_setTime(uint32_t seconds) {
    RTC_SR &= ~RTC_SR_TCE_MASK;          //!< desabilita o modulo para atualizar prescaler e contador
    RTC_TPR = (seconds * 1000) % 32768;  //!< reajustar segundos para a frequencia 1kHz
    RTC_TSR = (seconds * 1000) / 32768;
    RTC_SR |= RTC_SR_TCE_MASK;  //!< habilitar o modulo
}

void RTC_habilita() {
    RTC_SR |= RTC_SR_TCE_MASK;  // habilita o relogio
}

void RTC_desabilita() {
    RTC_SR &= ~RTC_SR_TCE_MASK;  // desabilita o relogio
}
