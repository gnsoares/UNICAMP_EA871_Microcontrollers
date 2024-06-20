/**
 * @file game.c
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief
 * @date 2024-06-15
 */

#include "game.h"

#include <math.h>

#include "ISR.h"
#include "mcu.h"

// Macro to compute the absolute value of the difference between two values
#define ABS_DIFF(a, b) ((a) > (b) ? ((a) - (b)) : ((b) - (a)))

void game_loop(uint8_t sets_to_win) {
    player_t winner_match = PLAYER_NONE, winner_point = PLAYER_NONE;
    board_t *board = ISR_getBoard();
    float t1, t2;
    ISR_setState(LAUNCH_BALL);
    t1 = get_time();
    while (1) {
        switch (ISR_getState()) {
            case PREPARA_INICIO:
                board_reset(board);
                // TODO: seta tela de inicio OLED
                // TODO: habilita interrupt IRQA12
                ISR_setState(INICIO);
                board_init_LCD(board);
                break;
            case INICIO:
                // do nothing
                break;
            case LAUNCH_BALL:
                board_reset_ball(board);
                ISR_setState(PLAYER_TURN);
                // TODO: enable interrupt of button of the player where the ball went towards
                // player = player where the ball went towards;
                break;
            case PLAYER_TURN:
                t2 = get_time();
                board_update(board, t2 - t1);
                board_display(board);
                winner_point = board_check_winner_point(board);
                t1 = t2;
            case LCD_UPDATE:
                board_update_score(board, winner_point);
                winner_match = board_check_winner_match(board, sets_to_win);
                ISR_setState(winner_match != PLAYER_NONE ? WIN_SCREEN : LAUNCH_BALL);
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

void board_reset(board_t *board) {
    board->ball_pos.x = 0;
    board->ball_pos.y = 0;
    board->ball_vel.x = 0;
    board->ball_vel.y = 0;
    board->score[0].sets = 0;
    board->score[1].sets = 0;
    board->score[0].games = 0;
    board->score[1].games = 0;
    board->score[0].points = 0;
    board->score[1].points = 0;
    board->bounces = 0;
}

void board_update(board_t *board, uint8_t dt) {
    float v_prev;
    // x_{k} = x_{k-1} + vx * dt
    board->ball_pos.x += board->ball_vel.x * dt;
    // vy_{k} = vy_{k-1} + g * dt
    v_prev = board->ball_vel.y;
    board->ball_vel.y += G * dt;
    // dy = (vy_{k}^2 - vy_{k-1}^2) / 2g
    board->ball_pos.y += (board->ball_vel.y * board->ball_vel.y - v_prev * v_prev) / (2 * G);
    if (board->ball_pos.y >= SCREEN_HEIGHT - 7) {
        board->ball_pos.x = SCREEN_HEIGHT - 7;
        board->ball_vel.y = -board->ball_vel.y;
        board->bounces++;
    }
}

player_t board_check_winner_point(board_t *board) {
    uint8_t ball_is_out = board->ball_pos.x < 0 || board->ball_pos.x >= SCREEN_WIDTH;
    if (
        board->bounces > 1 ||                  // 2 bounces
        (board->bounces = 1 && (ball_is_out))  // bounce and out
    ) {
        return board->ball_pos.x > SCREEN_WIDTH / 2 ? PLAYER_1 : PLAYER_2;
    }
    if (board->ball_pos.x < 0) {  // out from player 2
        return PLAYER_1;
    }
    if (board->ball_pos.x >= SCREEN_WIDTH) {  // out from player 1
        return PLAYER_2;
    }
    return PLAYER_NONE;
}

player_t board_check_winner_match(board_t *board, uint8_t sets_to_win) {
    if (board->score[0].sets >= sets_to_win) {
        return PLAYER_1;
    }
    if (board->score[1].sets >= sets_to_win) {
        return PLAYER_2;
    }
    return PLAYER_NONE;
}

void board_reset_ball(board_t *board) {
    // TODO
    board->ball_pos.x = 20.6;
    board->ball_pos.y = 20.6;
}

void board_update_score(board_t *board, player_t player) {
    if (player != PLAYER_1 && player != PLAYER_2) {
        // ERROR
        return;
    }
    uint8_t p_idx = ((uint8_t)player) - 1;
    switch (board->score[p_idx].points) {
        case 0:
            board->score[p_idx].points = 15;
            break;
        case 15:
            board->score[p_idx].points = 30;
            break;
        case 30:
            board->score[p_idx].points = 40;
            break;
        case 40:
            // IMPROV: deuce
            board->score[p_idx].points = 0;
            switch (board->score[p_idx].games) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    board->score[p_idx].games += 1;
                    board_update_LCD_games(board);
                    break;
                case 5:
                    // IMPROV: 2+ games diff
                    board->score[p_idx].games += 1;
                    board_update_LCD_games(board);
                    board->score[p_idx].games = 0;
                    board->score[p_idx].sets += 1;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    board_update_LCD_points(board);
}

void board_init_LCD(board_t *board) {
    player_t player = PLAYER_1;
    uint8_t line_offset;
    char string[5];
    uint8_t empty_LCD[] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
    GPIO_LCD_escreve_string(0x00, empty_LCD);
    GPIO_LCD_escreve_string(0x40, empty_LCD);
    while (player != PLAYER_NONE) {
        line_offset = (player == PLAYER_1 ? 0x00 : 0x40);
        string[0] = 'P';
        string[1] = (player == PLAYER_1 ? '1' : '2');
        string[2] = ':';
        string[3] = ' ';
        string[4] = '\0';
        GPIO_LCD_escreve_string(line_offset | 0x00, (uint8_t *)string);
        player = (player == PLAYER_1 ? PLAYER_2 : PLAYER_NONE);
    }
}

void board_update_LCD_games(board_t *board) {
    player_t player = PLAYER_1;
    uint8_t line_offset, column_offset = 2 * (board->score[0].sets + board->score[1].sets) + 4;
    char string[3];
    int p_idx;
    while (player != PLAYER_NONE) {
        p_idx = PLAYER_1 - 1;
        line_offset = (player == PLAYER_1 ? 0x00 : 0x40);
        string[0] = '0' + board->score[p_idx].games;
        string[1] = ' ';
        string[2] = '\0';
        GPIO_LCD_escreve_string(line_offset | column_offset, (uint8_t *)string);
        player = (player == PLAYER_1 ? PLAYER_2 : PLAYER_NONE);
    }
}

void board_update_LCD_points(board_t *board) {
    player_t player = PLAYER_1;
    uint8_t line_offset;
    char string[3];
    int p_idx;
    while (player != PLAYER_NONE) {
        p_idx = PLAYER_1 - 1;
        line_offset = (player == PLAYER_1 ? 0x00 : 0x40);
        string[0] = '0' + board->score[p_idx].points / 10;
        string[1] = '0' + board->score[p_idx].points % 10;
        string[2] = '\0';
        GPIO_LCD_escreve_string(line_offset | 0x0e, (uint8_t *)string);
        player = (player == PLAYER_1 ? PLAYER_2 : PLAYER_NONE);
    }
}

void board_display(board_t *board) {
    uint8_t i, j, x, y;
    // court
    for (i = 8; i < SCREEN_WIDTH - 8; i++) {
        for (j = SCREEN_HEIGHT - 6; j < SCREEN_HEIGHT - 4; j++) {
            I2C_OLED_setPixel(i, j);
        }
    }
    for (i = SCREEN_WIDTH / 2 - 1; i < SCREEN_WIDTH / 2 + 1; i++) {
        for (j = SCREEN_HEIGHT - 24; j < SCREEN_HEIGHT - 6; j++) {
            I2C_OLED_setPixel(i, j);
        }
    }
    // ball
    x = (uint8_t)roundf(board->ball_pos.x);
    y = (uint8_t)roundf(board->ball_pos.y);
    for (i = x - 2; i < x + 3; i++) {
        for (j = y - 2 + ABS_DIFF(i, x); j < y + 3 - ABS_DIFF(i, x); j++) {
            I2C_OLED_setPixel(i, j);
        }
    }
    I2C_OLED_redisplay();
}
