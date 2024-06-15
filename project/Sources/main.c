#include "game.h"
#include "mcu.h"

int main(int argc, char const *argv[]) {
    config();
    // TODO: ask user for game config
    game_loop(2);
    return 0;
}
