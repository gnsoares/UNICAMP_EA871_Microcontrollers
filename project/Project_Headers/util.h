/*!
 * @file util.h
 * @brief Prototipos, macros e tipos de dados de util.c
 * @date 19/05/2023
 * @author Wu Shin-Ting
 * @author Mateus Henrique Silva Araujo
 */

#ifndef UTIL_H_
#define UTIL_H_

#define GPIO_PIN(x)  ((1)<<(x))

/**
 * @brief Enumeracao para a definicao 
 */
typedef enum boolean_tag {
	OFF,     /**< falso/apaga/desativa/liga */
	ON      /**< verdadeiro/acende/ativa/fecha */
} tipo_booleano;

typedef enum bool_tag {
	false,     /**< falso/apaga/desativa/liga */
	true      /**< verdadeiro/acende/ativa/fecha */
} bool;

/**
 * @brief espera em multiplos de 5us
 * @param[in] multiplos de 5us
 */
void pause_5us (uint32_t multiplos);

/**
 * @brief espera em decrementos de valores ate 0
 * @param[in] valor de decremento
 */
void pause (uint32_t valor);

/**
 * @brief Calcula a potencia de um numero inteiro
 * @param[in] base base de calculo da potencia
 * @param[in] exp expoente para o calculo da potencia
 */
uint16_t power(uint8_t base, uint8_t exp);

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

/**
 * @brief Converte um valor em ponto flutuante para uma string
 * @param[in] n valor em ponto flutuante a ser convertido
 * @param[out] res apontador para a string que recebera a conversao
 * @param[in] afterpoint quantidade de digitos apos a casa decimal
 */
void ftoa(float n, char* res, int afterpoint);

#endif /* UTIL_H_ */
