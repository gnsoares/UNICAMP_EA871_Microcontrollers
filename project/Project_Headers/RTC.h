/**
 * @file RTC.h
 * @brief Declaracao de prototipos, macros, tipos de dados relativos a RTC
 * @date 08/02/2024
 * @author Wu Shin-Ting
 */

#ifndef RTC_H_
#define RTC_H_

/**
 * @brief Inicializa RTC com LPO como a fonte de sinal de relogio.
 */
void RTClpo_init();

/**
 * @brief Le o tempo corrente em segundos
 * @param[out] seconds lidos
 * @return segundos lidos
 */
uint32_t RTClpo_getTime(uint32_t *seconds);

/**
 * @brief Setar o valor do contador de segundos do RTC
 * @param[in] seconds valor de tempo em segundos
 */
void RTClpo_setTime(uint32_t seconds);

/**
 * @brief Habilita RTC
 */
void RTC_habilita();

/**
 * @brief Desabilita RTC
 */
void RTC_desabilita();

#endif /* RTC_H_ */
