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
 * @param[in] NMI_IRQC modo de interrupcao para NMI
 * @param[in] IRQA5_IRQC modo de interrupcao para IRQA5
 * @param[in] IRQA12_IRQC modo de interrupcao para IRQA12
 * @param[in] prioridade de atendimento
 */
void GPIO_switches_init(uint8_t NMI_IRQC,
                        uint8_t IRQA5_IRQC,
                        uint8_t IRQA12_IRQC,
                        uint8_t prioridade);

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
