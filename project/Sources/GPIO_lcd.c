/**
 * @file GPIO_lcd.c
 * @brief Este modulo contem interface do LCD via modulo GPIO
 * @date 02/02/2024
 * @author Wu Shin Ting
 */

#include "GPIO_lcd.h"

#include "mcu.h"
#include "util.h"

void GPIO_LCD_ativa_con() {
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // habilita sinal de clock de PORTC

    // Configura os pinos conectados em Latch e LCD com funcao GPIO
    PORTC_PCR0 |= PORT_PCR_MUX(0x1);  // D0-D7 dos dados
    PORTC_PCR1 |= PORT_PCR_MUX(0x1);
    PORTC_PCR2 |= PORT_PCR_MUX(0x1);
    PORTC_PCR3 |= PORT_PCR_MUX(0x1);
    PORTC_PCR4 |= PORT_PCR_MUX(0x1);
    PORTC_PCR5 |= PORT_PCR_MUX(0x1);
    PORTC_PCR6 |= PORT_PCR_MUX(0x1);
    PORTC_PCR7 |= PORT_PCR_MUX(0x1);

    PORTC_PCR8 |= PORT_PCR_MUX(0x1);  // RS do LCD
    PORTC_PCR9 |= PORT_PCR_MUX(0x1);  // E do LCD

    // Configura o sentido do sinal nos pinos PORTC0-10 como saida
    GPIOC_PDDR |= GPIO_PDDR_PDD(GPIO_PIN(9) | GPIO_PIN(8) | GPIO_PIN(7) |
                                GPIO_PIN(6) | GPIO_PIN(5) | GPIO_PIN(4) | GPIO_PIN(3) |
                                GPIO_PIN(2) | GPIO_PIN(1) | GPIO_PIN(0));
}

void GPIO_LCD_set_RS(tipo_lcd_RS i) {
    if (i == COMANDO) {
        GPIOC_PCOR = GPIO_PIN(8);  //!< Seta o LCD no modo de comando
    } else if (i == DADO) {
        GPIOC_PSOR = GPIO_PIN(8);  //!< Seta o LCD no modo de dados
    }
}

void GPIO_LCD_escreve_byte(uint8_t c, uint16_t t) {
    /*!
     * Coloca os sinais do byte nos pinos PTC0-PTC7 (byte menos significativo da PORTC)
     */
    GPIOC_PDOR &= ~0xff;
    GPIOC_PDOR |= c;

    /*!
     * Envia um pulso de E de largura maior que 450ns
     */
    GPIOC_PSOR = GPIO_PIN(9);
    espera_2us(1);
    GPIOC_PCOR = GPIO_PIN(9);

    /*!
     * Aguarda pelo processamento
     */
    espera_2us(t);
}

void GPIO_LCD_init() {
    espera_2us(15000);  // espera por mais de 30ms: 30000us/2 = 15000

    GPIO_LCD_set_RS(COMANDO);
    GPIO_LCD_escreve_byte(0x38, 20);   // Function Set: 39us/2 = 20
    GPIO_LCD_escreve_byte(0x0C, 20);   // Display ON/OFF Control: 39us/2 = 20
    GPIO_LCD_escreve_byte(0x01, 765);  // Display Clear: 1530us/2 = 765
    GPIO_LCD_escreve_byte(0x06, 20);   //!< Entry mode set: 39us/2 = 20
}

void GPIO_LCD_escreve_string(uint8_t end, uint8_t* str) {
    // Codifica o endereco na instrucao "Set DDRAM Address".
    uint8_t tmp = 0b10000000 | end;

    // Seta end no registrador de endereco de DDRAM por um tempo de processamento
    //  maior que 39us (39/2 = 20)
    GPIO_LCD_set_RS(COMANDO);
    GPIO_LCD_escreve_byte(tmp, 20);

    // Grava os caracteres da Tabela de Fontes a partir do endereco setado.
    // O tempo de escrita de cada byte eh maior que 43us (43/2 = 22)
    GPIO_LCD_set_RS(DADO);
    while (*str != '\0') {
        GPIO_LCD_escreve_byte(*str, 22);
        str++;
    }
}
