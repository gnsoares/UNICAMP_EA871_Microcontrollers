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
 * @brief Setar o valor do contador de segundos do RTC
 * @param[in] seconds valor de tempo em segundos
 */
void RTClpo_setTime(uint32_t seconds);

#endif /* RTC_H_ */
