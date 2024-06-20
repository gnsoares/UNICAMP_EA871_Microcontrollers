/**
 * @file GPIO_switches.h
 * @brief Prototipos, macros e tipos de dados de GPIO_switches
 * @date 12/04/2024
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 */

#ifndef GPIO_SWITCHES_H_
#define GPIO_SWITCHES_H_

#include <stdint.h>

/**
 * @brief Inicializa botoeiras
 *
 * NAO habilita interrupcoes
 *
 * @param[in] prioridade prioridade de atendimento
 */
void GPIO_switches_init(uint8_t prioridade);

/**
 * @brief Desativa IRQ30, sem limpar as pendencias
 */
void GPIO_switches_NVIC_interrupt_desativa();

/**
 * @brief Reativa IRQ30, sem limpar as pendencias
 */
void GPIO_switches_NVIC_interrupt_ativa();

void GPIO_switches_IRQAn_interrupt_desativa(uint8_t n);
void GPIO_switches_IRQAn_interrupt_ativa(uint8_t n, uint8_t IRQC);

#endif /* GPIO_SWITCHES_H_ */
