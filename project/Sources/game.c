#include "game.h"

#include "ISR.h"
#include "mcu.h"

void game_loop(uint8_t sets_to_win) {
    player_t player = PLAYER_1, winner = PLAYER_NONE;
    board_t board;
    float t1, t2;
    board_init(&board);
    while (1) {
        t1 = get_time();
        while ((winner = board_check_winner(&board, sets_to_win)) == PLAYER_NONE) {
            t2 = get_time();
            board_update(&board, t2 - t1);
            // TODO: wait for button press
            player = (player == PLAYER_1 ? PLAYER_2 : PLAYER_1);
            t1 = t2;
        }
        // TODO: show winner screen
        // TODO: play again?
    }
}

void board_init(board_t *board) {
    board->ball_pos.x = 0;
    board->ball_pos.y = 0;
    // TODO: init random vel
    board->ball_vel.x = 0;
    board->ball_vel.y = 0;
    board->score.p1_sets = 0;
    board->score.p2_sets = 0;
    board->score.p1_games = 0;
    board->score.p2_games = 0;
    board->score.p1_points = 0;
    board->score.p2_points = 0;
}

void board_update(board_t *board, float dt) {
    uint8_t v_prev;
    // x_{k} = x_{k-1} + vx * dt
    board->ball_pos.x += board->ball_vel.x * dt;
    // vy_{k} = vy_{k-1} + g * dt
    v_prev = board->ball_vel.y;
    board->ball_vel.y += G * dt;
    // dy = (vy_{k}^2 - vy_{k-1}^2) / 2g
    board->ball_pos.y += (board->ball_vel.y * board->ball_vel.y - v_prev * v_prev) / (2 * G);
}

player_t board_check_winner(board_t *board, uint8_t sets_to_win) {
    if (board->score.p1_sets >= sets_to_win) {
        return PLAYER_1;
    }
    if (board->score.p2_sets >= sets_to_win) {
        return PLAYER_2;
    }
    return PLAYER_NONE;
}
