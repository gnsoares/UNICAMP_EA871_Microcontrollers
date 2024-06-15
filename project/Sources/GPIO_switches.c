/**
 * @file GPIO_switches.c
 * @brief Definicao das funcoes declaradas em GPIO_switches.h
 * @date 12/04/2024
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 */

#include "derivative.h"
#include "util.h"

void GPIO_initSwitches(uint8_t NMI_IRQC,
                       uint8_t IRQA5_IRQC,
                       uint8_t IRQA12_IRQC,
                       uint8_t prioridade) {
    // Habilita o clock do modulo PORTA para botoeiras
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

    // Funcao GPIO
    // Muda modo de multiplexacao para GPIO para contornar interrupcoes indesejaveis
    // limpar os bits 8:10
    PORTA_PCR4 &= ~PORT_PCR_MUX(0x7);
    PORTA_PCR5 &= ~PORT_PCR_MUX(0x7);
    PORTA_PCR12 &= ~PORT_PCR_MUX(0x7);
    // setar alternativa 1
    PORTA_PCR4 |= PORT_PCR_MUX(0x1);
    PORTA_PCR5 |= PORT_PCR_MUX(0x1);
    PORTA_PCR12 |= PORT_PCR_MUX(0x1);

    // Sentido do sinal: entrada
    GPIOA_PDDR &= ~(GPIO_PIN(4) | GPIO_PIN(5) | GPIO_PIN(12));

    // Configura modo de interrupcao para pinos PTA4, PTA5 e PTA12
    // limpar os bits 16:19
    PORTA_PCR4 &= ~PORT_PCR_IRQC(0xF);
    PORTA_PCR5 &= ~PORT_PCR_IRQC(0xF);
    PORTA_PCR12 &= ~PORT_PCR_IRQC(0xF);
    // limpa flag de interrupcao + configura evento de interrupcao
    PORTA_PCR4 |= PORT_PCR_ISF_MASK | PORT_PCR_IRQC(NMI_IRQC);      // modo de interrupcao
    PORTA_PCR5 |= PORT_PCR_ISF_MASK | PORT_PCR_IRQC(IRQA5_IRQC);    // modo de interrupcao
    PORTA_PCR12 |= PORT_PCR_ISF_MASK | PORT_PCR_IRQC(IRQA12_IRQC);  // modo de interrupcao

    /**
     * Configura o modulo NVIC: habilita IRQ 30 e limpa pendencias IRQ 30
     */
    NVIC_ISER = NVIC_ISER_SETENA(GPIO_PIN(30));  // Habilita interrupcao PORTA
    NVIC_ICPR = NVIC_ICPR_CLRPEND(GPIO_PIN(30));

    /**
     * Configura o modulo NVIC: seta prioriodade para IRQ30
     * registrador NVIC_IPR7 mapeado em 0xe000e41cu (Tabela 3-7/p. 54 do Manual)
     */
    NVIC_IPR7 |= NVIC_IP_PRI_30(prioridade << 6);

    return;
}

void GPIO_desativaSwitchesNVICInterrupt() {
    NVIC_ICER = NVIC_ICER_CLRENA(GPIO_PIN(30));  // Desativa IRQ30 (PORTA)
}

void GPIO_reativaSwitchesNVICInterrupt() {
    NVIC_ISER = NVIC_ISER_SETENA(GPIO_PIN(30));  // Reativa IRQ30 (PORTA)
}
