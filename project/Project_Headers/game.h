/**
 * @file mcu.h
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief
 * @date 2024-06-15
 */

#ifndef _GAME_H
#define _GAME_H

#include <stdint.h>

#define G 9.81

typedef enum {
    PLAYER_NONE,
    PLAYER_1,
    PLAYER_2
} player_t;

typedef struct {
    float x;
    float y;
} vector_2d_t;

typedef struct {
    uint8_t sets;
    uint8_t games;
    uint8_t points;
} score_t;

typedef struct {
    vector_2d_t ball_pos;
    vector_2d_t ball_vel;
    score_t score[2];
} board_t;

void game_loop(uint8_t sets_to_win);
void board_reset(board_t *board);
void board_update(board_t *board, uint8_t dt);
player_t board_check_winner_match(board_t *board, uint8_t sets_to_win);
void board_reset_ball(board_t *board);
void board_update_score(board_t *board, player_t player);
void board_init_LCD(board_t *board);
void board_update_LCD_games(board_t *board);
void board_update_LCD_points(board_t *board);
void board_display(board_t *board);

#endif
