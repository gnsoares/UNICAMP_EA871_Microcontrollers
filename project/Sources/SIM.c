/**
 * @name SIM.c
 * @brief Definicao de funcoes de derivacao de sinais de relogio
 * @date Jan 16, 2023
 * @author Wu Shin-Ting
 */
#include "mcu.h"

void SIM_setaOUTDIV4(uint8_t OUTDIV4) {
    /*
     * Configurar a frequencia do clock de barramento atraves do divisor da frequencia
     * do sinal MCGOUTCLK  (clock de barramento = MCGOUTCLK/(OUTDIV1*OUTDIV4))
     */
    SIM_CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV4(0b111);  // setar OUTDIV4
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV4(OUTDIV4);
}

void SIM_setaFLLPLL(uint8_t pll) {
    /*
     * Selecionar o modo de geracao por laco de sincronismo
     */
    if (pll) {
        SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;  // setar PLL
    } else {
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;  // setar FLL
    }
}

void SIM_setaTPMSRC(uint8_t src) {
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(src);
}
