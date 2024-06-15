/**
 * @file TPM.h
 * @brief Prototipos, macros e tipos de dados referentes a TPM.
 * @date Jan 27, 2023
 * @author Wu Shin Ting
 */
#ifndef TPM_H_
#define TPM_H_

#include <stdint.h>

/**
 * @brief Configurar sinais de relogio e pinos para TPM1_CH1
 *  TPM1_CH1
 */
void TPM_config_basica();

/**
 * @brief Configurar o contador de TPMx
 * @param[in] x identificador do TPM (0, 1 ou 2)
 * @param[in] mod contagem maxima
 * @param[in] trigger fonte de disparos para controle das contagens (3.8.1.3/p.86)
 * @param[in] crot resetar CNT no disparo
 * @param[in] csoo parar CNT em TOF
 * @param[in] csot ativar contagem no disparo
 * @param[in] dma habilitar transferencias por DMA nos eventos de OVF
 * @param[in] cpwms modo de contagem (0=crescente; 1=crescente-decrescente)
 * @param[in] ps divisor pre-scaler
 */
void TPM_config_especifica(uint8_t x, uint16_t mod, uint8_t trigger, uint8_t crot, uint8_t csoo, uint8_t csot,
                           uint8_t dma, uint8_t cpwms, uint8_t ps);

/**
 * @brief Configurar um canal n de TPMx
 * @param[in] x identificador do TPM (0, 1 ou 2)
 * @param[in] n identificador do canal
 * @param[in] MS_ELS modo de operacao
 * @param[in] valor no registrador CnV
 */
void TPM_CH_config_especifica(uint8_t x, uint8_t n, uint8_t MS_ELS, uint16_t valor);

/**
 * @brief Habilitar IRQ
 * @param[in] irq linha de solicitacao
 * @param[in] prioridade de atendimento (0 a 3)
 */
void TPM_habilitaNVICIRQ(uint8_t irq, uint8_t prioridade);

/**
 * @brief Habilitar interrupcao pelo evento TOF do modulo TPMx
 * @param[in] x identificador de TPM
 */
void TPM_habilitaInterrupTOF(uint8_t x);

/**
 * @brief Desabilitar interrupcao pelo evento TOF do modulo TPMx
 * @param[in] x identificador de TPM
 */
void TPM_desabilitaInterrupTOF(uint8_t x);

/**
 * @brief Habilitar interrupcao pelo evento do canal do modulo TPMx
 * @param[in] x identificador de TPM
 * @param[in] n identificador de canal
 */
void TPM_habilitaInterrupCH(uint8_t x, uint8_t n);

/**
 * @brief Habilitar interrupcao pelo evento do canal do modulo TPMx
 * @param[in] x identificador de TPM
 * @param[in] n identificador de canal
 */
void TPM_desabilitaInterrupCH(uint8_t x, uint8_t n);

/**
 * @brief Setar Modulo do TPMx
 * @param[in] x identificador de TPM
 */
void TPM_setaMOD(uint8_t x, uint16_t mod);

/**
 * @brief Setar o valor no canal
 * @param[in] x identificador do modulo TPM
 * @param[in] n identificador do canal
 */
void TPM_setaCnV(uint8_t x, uint8_t n, uint16_t valor);

/**
 * @brief Ativa contador de um modulo TPM
 *
 * @param[in] x identificador do modulo TPM
 */
void TPM_ativaContador(uint8_t x);

/**
 * @brief Desativa contador de um modulo TPM
 *
 * @param[in] x identificador do modulo TPM
 */
void TPM_desativaContador(uint8_t x);

#endif /* TPM_H_ */
