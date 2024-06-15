/**
 * @file GPIO_switches.h
 * @brief Prototipos, macros e tipos de dados de GPIO_switches
 * @date 12/04/2024
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 */

#ifndef GPIO_SWITCHES_H_
#define GPIO_SWITCHES_H_
// Inclusao dos tipos de dados padronizados pelo ISO C99
#include <stdint.h>

/**
 * @brief Inicializa botoeiras
 * @param[in] NMI_IRQC modo de interrupcao para NMI
 * @param[in] IRQA5_IRQC modo de interrupcao para IRQA5
 * @param[in] IRQA12_IRQC modo de interrupcao para IRQA12
 * @param[in] prioridade de atendimento
 */
void GPIO_initSwitches(uint8_t NMI_IRQC,
                       uint8_t IRQA5_IRQC,
                       uint8_t IRQA12_IRQC,
                       uint8_t prioridade);

/**
 * @brief Desativa IRQ30, sem limpar as pendencias
 */
void GPIO_desativaSwitchesNVICInterrupt();

/**
 * @brief Reativa IRQ30, sem limpar as pendencias
 */
void GPIO_reativaSwitchesNVICInterrupt();

#endif /* GPIO_SWITCHES_H_ */
