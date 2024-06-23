/**
 * @file main.c
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief Adaptacao do jogo Tennis for Two para tela OLED e o microcontrolador FRDM-KL25Z4
 * @date 2024-06-15
 */

#include "game.h"
#include "mcu.h"

int main(int argc, char const *argv[]) {
    config();
    // IMPROV: ask user for game config
    game_loop(2);
    return 0;
}
