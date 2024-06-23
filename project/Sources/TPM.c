/*!
 * @file TPM.c
 * @brief Este modulo contem interface dos m�dulos TPMx.
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @date 26/01/2023
 */

#include "mcu.h"

static TPM_MemMapPtr TPM[] = TPM_BASE_PTRS;

void TPM_config_basica() {
    /**
     * Habilita os sinais de rel�gio para TPM1
     */
    SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;

    /**
     * Configura pinos PT
     */
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;  // habilita sinais de relogio

    PORTE_PCR21 |= (PORT_PCR_ISF_MASK |  // TPM1_CH1
                    PORT_PCR_MUX(0x3) |
                    PORT_PCR_DSE_MASK);  // drive strength enable
    // e.g. 18 mA vs. 5 mA @ > 2.7 V,

    return;
}

void TPM_config_especifica(uint8_t x, uint16_t mod, uint8_t trigger, uint8_t crot, uint8_t csoo, uint8_t csot,
                           uint8_t dma, uint8_t cpwms, uint8_t ps) {
    /**
     * Configurar disparo/recarga do contador CNT
     */
    uint8_t temp = trigger & 0b1111;

    if (temp != 0b0010 && temp != 0b0011 && temp != 0b0110 &&
        temp != 0b0111 && temp != 0b1011 && temp != 0b1111) {
        TPM[x]->CONF &= ~((0b1111 << 24) | (0b111 << 16));
        TPM[x]->CONF |= TPM_CONF_TRGSEL(trigger);      // Tabela 3-38/p. 86
        if (crot) TPM[x]->CONF |= TPM_CONF_CROT_MASK;  // Resetar CNT no disparo
        if (csoo) TPM[x]->CONF |= TPM_CONF_CSOO_MASK;  // CNT para em TOF
        if (csot) TPM[x]->CONF |= TPM_CONF_CSOT_MASK;  // Ativar contador no disparo
    }
    /**
     * Resetar o contador fazendo um acesso de escrita
     */
    TPM[x]->CNT = 0;

    /**
     * Configurar periodo do contador T = PS*MOD/freq.
     */
    TPM[x]->SC &= ~TPM_SC_PS(0b111);
    TPM[x]->SC |= TPM_SC_PS(ps);
    if (dma) TPM[x]->SC |= TPM_SC_DMA_MASK;
    if (cpwms) TPM[x]->SC |= TPM_SC_CPWMS_MASK;

    /**
     * Configurar a contagem maxima
     */
    TPM[x]->MOD = TPM_MOD_MOD(mod);

    TPM[x]->SC |= TPM_SC_CMOD(1);  // ativar o contador LPTPM
    return;
}

void TPM_CH_config_especifica(uint8_t x, uint8_t n, uint8_t MS_ELS, uint16_t valor) {
    /**
     * Configurar o modo de operacao do canal
     */
    TPM[x]->CONTROLS[n].CnSC &= ~(0b1111 << 2);
    TPM[x]->CONTROLS[n].CnSC |= (MS_ELS << 2);

    /**
     * Inicializar com valor 0 no contador
     */
    TPM[x]->CONTROLS[n].CnV = TPM_CnV_VAL(valor);

    return;
}

void TPM_habilitaNVICIRQ(uint8_t irq, uint8_t prioridade) {
    /**
     * Configura o modulo NVIC: habilita IRQ irq
     */
    NVIC_ISER = NVIC_ISER_SETENA(1 << irq);

    /**
     * Configura o modulo NVIC: limpa pendencias IRQ irq
     */
    NVIC_ICPR = NVIC_ICPR_CLRPEND(1 << irq);

    /**
     * Configura o modulo NVIC: seta prioriodade 3 para IRQ
     * registrador NVIC_IPRn n = irq/4(Tabela 3-7/p. 53 do Manual)
     * campo dentro do registrador: (irq%4)*8
     */
    NVIC_IP_REG(NVIC_BASE_PTR, irq / 4) |= (prioridade << 6) << (irq % 4 * 8);  //(Secao 3.3.2.1/p. 53 do Manual)

    return;
}

void TPM_habilitaInterrupTOF(uint8_t x) {
    TPM[x]->SC |= (TPM_SC_TOF_MASK |   // resetar flag
                   TPM_SC_TOIE_MASK);  // habilitar a interrupcao TOF
}

void TPM_desabilitaInterrupTOF(uint8_t x) {
    TPM[x]->SC &= ~TPM_SC_TOIE_MASK;  // habilitar a interrupcao TOF
}

void TPM_setaMOD(uint8_t x, uint16_t mod) {
    TPM[x]->MOD = TPM_MOD_MOD(mod);
}

void TPM_setaCnV(uint8_t x, uint8_t n, uint16_t valor) {
    TPM[x]->CONTROLS[n].CnV = TPM_CnV_VAL(valor);
}
