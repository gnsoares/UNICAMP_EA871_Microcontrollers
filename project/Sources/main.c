#include "game.h"
#include "mcu.h"

int main(int argc, char const *argv[]) {
    config();
    // TODO: ask user for game config
    game_loop(2);

    	    for (;;) {
	                TPM_habilitaInterrupTOF(1);
	                espera_2us(1500000);  // espera 3s para leitura
	                TPM_desabilitaInterrupTOF(1);
	                TPM_setaCnV(1, 1, 0);  // para sinal sonoro
    return 0;
}
