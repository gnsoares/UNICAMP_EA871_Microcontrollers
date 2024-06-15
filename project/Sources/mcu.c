#include "mcu.h"

void config(void) {
    	SIM_setaFLLPLL(0);
	    SIM_setaTPMSRC(0b01);
	    TPM_config_basica();
	    TPM_config_especifica(0, 16384, 0b1111, 0, 0, 0, 0, 0, 0b111);  // (16384*128)/20971520 = 0.1s
	    TPM_CH_config_especifica(0, 2, 0b0010, 0);  // captura borda de descida de PTA5
	    TPM_config_especifica(1, 16384, 0b1111, 0, 0, 0, 0, 0, 0b111);  // (16384*128)/20971520 = 0.1s
	    TPM_CH_config_especifica(1, 0, 0b0010, 0);  // captura borda de descida de PTA12
	    TPM_CH_config_especifica(1, 1, 0b1010, 0);  // PWM
	    TPM_config_especifica(2, 32768, 0b1111, 0, 0, 0, 0, 0, 0b000);  // (32768*1)/20971520 = 1.5625ms
	    TPM_CH_config_especifica(2, 0, 0b0101, 0);  // output compare toggle
	    TPM_habilitaNVICIRQ(17, 3);  // TPM0
	    TPM_habilitaNVICIRQ(18, 3);  // TPM1
	    TPM_habilitaNVICIRQ(19, 3);  // TPM2
    // TODO
}

float get_time(void) {
    // TODO
}
