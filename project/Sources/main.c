/**
 * @file main.c
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief
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
