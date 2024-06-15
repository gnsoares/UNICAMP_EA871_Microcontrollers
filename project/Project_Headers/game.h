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
    uint8_t x;
    uint8_t y;
} vector_2d_t;

typedef struct {
    uint8_t p1_sets;
    uint8_t p2_sets;
    uint8_t p1_games;
    uint8_t p2_games;
    uint8_t p1_points;
    uint8_t p2_points;
} score_t;

typedef struct {
    vector_2d_t ball_pos;
    vector_2d_t ball_vel;
    score_t score;
} board_t;

void game_loop(uint8_t sets_to_win);
void board_init(board_t *board);
void board_update(board_t *board, float dt);
player_t board_check_winner(board_t *board, uint8_t sets_to_win);
void board_reset_ball(board_t *board);

#endif
