#include "mcu.h"

void config(void) {
    SIM_setaFLLPLL(0);  // MCGFLLCLK

    // Define a frequencia do sinal do barramento
    SIM_setaOUTDIV4(0b001);  // bus clock = 20971520/2 = 10485760

    // Seta a fonte do sinal ERCLK32K em LPO 1kHz
    OSC_LPO1kHz();

    // Inicializa conexao com LCD
    GPIO_ativaConLCD();

    // Inicializa LCD
    GPIO_initLCD();

    // Inicializa botoes
    GPIO_initSwitches(
        0b1010,  // NMI: falling edge
        0b1010,  // IRQA5: falling edge
        0b1010,  // IRQA12: falling edge
        2        // prioridade = 3 (mais baixa)
    );

    // Inicializa o modulo RTC com fonte LPO
    RTClpo_init();

    SIM_setaTPMSRC(0b01);  // MCGFLLCLK clock or MCGPLLCLK/2
    TPM_config_basica();
    TPM_config_especifica(1,          // TPM1
                          16384,      // MOD
                          0b1111,     // Trigger = NO TRIGGER
                          0,          // Reset On Trigger DISABLE
                          0,          // Stop On Overflow DISABLE
                          0,          // Start On Trigger DISABLE
                          0,          // DMA DISABLE
                          0,          // Counting Mode = ASCENDING
                          0b111       // log2(PS) => PS = 2^7 == 128
    );                                // (16384*128)/20971520 = 0.1s
    TPM_CH_config_especifica(1,       // TPM1
                             1,       // CH1
                             0b1010,  // Mode = Edge-aligned PWM
                             0        // TPM1_C1V
    );
    TPM_habilitaNVICIRQ(18, 3);  // TPM1
}

float get_time(void) {
    // TODO
}
