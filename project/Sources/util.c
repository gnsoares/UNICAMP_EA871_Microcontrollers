/**
 * @file util.c
 * @brief Definicao das funcoes uteis do projeto
 * @date 19/05/2023
 * @author Mateus Henrique Silva Araujo   
 */
//Inclusao dos tipos de dados padronizados pelo ISO C99
#include "stdint.h"


void pause_5us (uint32_t multiplos)
{

		__asm__ (
				"iteracao: \n\t"
					"mov r2, #4 \n\t"
				"laco: \n\t" 
					"mov r3, #5 \n\t"
					"orr r3, r0 \n\t"
					"and r3, r0 \n\t"     
					"lsr r3, #1 \n\t"
					"asr r3, #1 \n\t"
					"and r3, r0 \n\t"
					"lsr r3, #1 \n\t"
					"asr r3, #1 \n\t"
					"asr r3, #1 \n\t"   
					"and r3, r0 \n\t"     
					"lsr r3, #1 \n\t"     
					"asr r3, #1 \n\t" 
					"asr r3, #1 \n\t"    
					"and r3, r0 \n\t"     
					"lsr r3, #1 \n\t"     
					"asr r3, #1 \n\t" 
					"asr r3, #1 \n\t"    
					"and r3, r0 \n\t"     
					"lsr r3, #1 \n\t"     
					"asr r3, #1 \n\t" 
					"asr r3, #1 \n\t"
					"sub r2, #1 \n\t"					 
					"cmp r2, #0 \n\t"
					"bne laco \n\t"
					"rev r3,r3 \n\t"
					"lsl r3,#0 \n\t"
					"sub %0,#1 \n\t"
					"cmp %0, #0 \n\t"
					"bne iteracao \n\t"
				 :
				 : "r" (multiplos)
				 : "r2","r3","cc"
		);
}


void pause (uint32_t valor)
{
	while (valor) valor--;
}


uint16_t power(uint8_t base, uint8_t exp){
	if (exp==0)return 1;
	if (exp==1)return base;
	while(exp>1){
		base*=base;
		exp--;
	}
	return base;
}


void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    do {
    	str[i++] = (x % 10) + '0';
    	x = x / 10;
    }
    while (x);

    while (i < d) str[i++] = '0';
    reverse(str, i);
    str[i] = '\0';
    return i;
}

void ftoa(float n, char* res, int afterpoint){ //Converte um float em uma string com afterpoint digitos depois do ponto
	
	// Extrai a parte inteira
	int ipart = (int)n;
	  
	// Extrai a parte em ponto flutuante
	float fpart = n - (float)ipart;
	
	// Converte a parte inteira para string
	int i = intToStr(ipart, res, 0);
	  
	if (afterpoint != 0) {
	   res[i] = '.'; // Adiciona o ponto
	   fpart = fpart * power(10, afterpoint); //Desloca a parte significativa depois do ponto

	   if (fpart - (int)fpart >= 0.5) fpart += 1.0;
	   intToStr((int)fpart, res + i + 1, afterpoint);
	}

	return;
}

