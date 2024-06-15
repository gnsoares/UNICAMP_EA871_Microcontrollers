/**
 * @file util.c
 * @brief Definicao das funcoes uteis do projeto
 * @date 19/05/2023
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 */

#include <stdint.h>

void espera_2us(uint32_t n) __attribute__((naked));
void espera_2us(uint32_t n) {
    asm volatile(
        "cmp r0, #0 \n\t"
        "beq espera_ret \n\t"
        "espera_loop: \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "add r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "sub r0, #1 \n\t"
        "cmp r0, #0 \n\t"
        "bne espera_loop \n\t"
        "espera_ret: \n\t"
        "bx lr"
        :
        :
        :);
}

void reverse(char* str, int len) {
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d) {
    int i = 0;
    do {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    } while (x);

    while (i < d) str[i++] = '0';
    reverse(str, i);
    str[i] = '\0';
    return i;
}
