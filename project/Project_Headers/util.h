/*!
 * @file util.h
 * @brief Prototipos, macros e tipos de dados de util.c
 * @date 19/05/2023
 * @author Wu Shin-Ting
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#define GPIO_PIN(x) ((1) << (x))

/**
 * @brief Executa em multiplos de 2us sem outras alteracoes
 *
 * Tempo final de execucao sera n * 2us
 *
 * @param[in] n multiplica 2us
 */
void espera_2us(uint32_t n);

/**
 * @brief Inverte uma string de tamanho especificado
 * @param[out] str apontador para a string a ser invertida
 * @param[in] len tamanho da string invertida
 */
void reverse(char* str, int len);

/**
 * @brief Converte um inteiro para um String
 * @param[in] x inteiro para ser convertido
 * @param[out] str apontador para a string que recebera o inteiro
 * @param[in] d numero minimo de digitos
 */
int intToStr(int x, char str[], int d);

#endif /* UTIL_H_ */
