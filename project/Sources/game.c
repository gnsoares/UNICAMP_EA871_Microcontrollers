#include "game.h"

#include "ISR.h"
#include "mcu.h"

void game_loop(uint8_t sets_to_win) {
    player_t player = PLAYER_1, winner = PLAYER_NONE;
    board_t board;
    float t1, t2;
    board_init(&board);
    t1 = get_time();
    while (1) {
        switch (ISR_getState()) {
            case PREPARA_INICIO:
                // TODO: seta tela de inicio OLED
                // TODO: habilita interrupt IRQA12
                ISR_setState(INICIO);
                break;
            case INICIO:
                // do nothing
                break;
            case LAUNCH_BALL:
                board_reset_ball(&board);
                ISR_setState(PLAYER_TURN);
                break;
            case PLAYER_TURN:
                t2 = get_time();
                board_update(&board, t2 - t1);
                // TODO: wait for button press (IRQHandler)
                player = (player == PLAYER_1 ? PLAYER_2 : PLAYER_1);
                t1 = t2;
            case LCD_UPDATE:
                // TODO: update lcd with points
                winner = board_check_winner(&board, sets_to_win);
                ISR_setState(winner != PLAYER_NONE ? WIN_SCREEN : LAUNCH_BALL);
                break;
            case WIN_SCREEN:
                // TODO: seta tela de vencedor OLED
                ISR_setState(WIN_VISU);
            case WIN_VISU:
                // TODO: espera 5s
                ISR_setState(PREPARA_INICIO);
                break;
            default:
                break;
        }
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

void board_reset_ball(board_t *board) {
    // TODO
}
