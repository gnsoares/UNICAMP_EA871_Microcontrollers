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

#define PIXELS_P_METER 4.712
#define G 9.81 * PIXELS_P_METER / 1000 / 1000  // pixels / ms^2
#define V0 5 * PIXELS_P_METER / 1000           // pixels / ms

#define NET_TOP SCREEN_HEIGHT - 24
#define NET_LEFT SCREEN_WIDTH / 2 - 1
#define NET_RIGHT SCREEN_WIDTH / 2 + 1
#define FLOOR_LEVEL SCREEN_HEIGHT - 6
#define FLOOR_HEIGHT 2

void game_loop(uint8_t sets_to_win) {
    player_t winner_match = PLAYER_NONE, winner_point = PLAYER_NONE;
    board_t *board = ISR_getBoard();
    uint32_t t1, t2;  // ms
    ISR_setState(PREPARA_INICIO);

    while (1) {
        switch (ISR_getState()) {
            case PREPARA_INICIO:
                board_reset(board);
                // TODO: seta tela de inicio OLED
                GPIO_switches_IRQAn_interrupt_ativa(12, BTN_IRQC);
                ISR_setState(INICIO);
                // TODO: make this keep the last score
                board_init_LCD(board);
                break;
            case INICIO:
                board_startscreen_display()
                break;
            case LAUNCH_BALL:
                board_reset_ball(board);
                ISR_setState(PLAYER_TURN);
                reset_time();
                t1 = get_time();
                if (board->ball_vel.x < 0) {
                    GPIO_switches_IRQAn_interrupt_ativa(4, BTN_IRQC);
                    ISR_setPlayer(PLAYER_1);
                } else {
                    GPIO_switches_IRQAn_interrupt_ativa(5, BTN_IRQC);
                    ISR_setPlayer(PLAYER_2);
                }
                break;
            case PLAYER_TURN:
                t2 = get_time();
                if (t1 > t2) {
                    reset_time();
                    t1 = get_time();
                    break;
                }
                board_update(board, t2 - t1);
                board_display(board);
                winner_point = board_check_winner_point(board);
                ISR_setState(LCD_UPDATE);
                t1 = t2;
                break;
            case LCD_UPDATE:
                if (winner_point == PLAYER_NONE) {
                    ISR_setState(PLAYER_TURN);
                    break;
                }
                // jogador venceu ponto: desabilitar botoes
                GPIO_switches_IRQAn_interrupt_desativa(4);
                GPIO_switches_IRQAn_interrupt_desativa(5);

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
    board->bounces_left = 0;
    board->bounces_right = 0;
}

void board_update(board_t *board, uint32_t dt) {
    static region_t region_prev = MIDDLE;
    float x_prev = board->ball_pos.x, y_prev = board->ball_pos.y;

    // x_{k} = x_{k-1} + vx * dt
    board->ball_pos.x += board->ball_vel.x * dt;
    // S2 = S1 + V*t + a*t^2/2
    board->ball_pos.y += board->ball_vel.y * dt + G * dt * dt / 2;
    // vy_{k} = vy_{k-1} + g * dt
    board->ball_vel.y += G * dt;

    if (board->ball_pos.y >= FLOOR_LEVEL - 1 && board->ball_vel.y > 0) {
        // acertou o chao: quica
        board->ball_pos.y = FLOOR_LEVEL - 1;
        board->ball_vel.y *= -.8;
        if (board->ball_pos.x < SCREEN_WIDTH / 2) {
            board->bounces_left++;
        } else {
            board->bounces_right++;
        }
    }

    if (y_prev >= NET_TOP || board->ball_pos.y >= NET_TOP) {
        // acertou a rede: reflete
        if (board->ball_pos.x >= NET_LEFT && board->ball_pos.x <= NET_RIGHT) {
            board->ball_vel.x *= -.8;
        } else if (x_prev < SCREEN_WIDTH / 2 && board->ball_pos.x > SCREEN_WIDTH / 2) {
            // acertou da esquerda
            board->ball_vel.x *= -.8;
            board->ball_pos.x = NET_LEFT;
        } else if (x_prev > SCREEN_WIDTH / 2 && board->ball_pos.x < SCREEN_WIDTH / 2) {
            // acertou da direita
            board->ball_vel.x *= -.8;
            board->ball_pos.x = NET_RIGHT;
        }
    } else {
        // passou por cima da rede: reseta bounces e gerencia quem pode rebater a bola
        if (region_prev != MIDDLE && board->ball_pos.x > SCREEN_WIDTH / 2 - 5 && board->ball_pos.x < SCREEN_WIDTH / 2 + 5) {
            // bola no meio
            region_prev = MIDDLE;
            board->bounces_left = 0;
            board->bounces_right = 0;
            GPIO_switches_IRQAn_interrupt_desativa(4);
            GPIO_switches_IRQAn_interrupt_desativa(5);
        } else if (region_prev == MIDDLE && board->ball_pos.x > SCREEN_WIDTH / 2 + 5) {
            // passou da esquerda para a direita
            region_prev = RIGHT;
            GPIO_switches_IRQAn_interrupt_ativa(5, BTN_IRQC);
        } else if (region_prev == MIDDLE && board->ball_pos.x < SCREEN_WIDTH / 2 - 5) {
            // passou da direita para a esquerda
            region_prev = LEFT;
            GPIO_switches_IRQAn_interrupt_ativa(4, BTN_IRQC);
        }
    }
}

player_t board_check_winner_point(board_t *board) {
    // TODO: take into consideration court dimensions
    uint8_t ball_is_out = board->ball_pos.x < 0 || board->ball_pos.x >= SCREEN_WIDTH;
    if (board->bounces_left > 1 || (board->bounces_left == 1 && (ball_is_out))) {
        return PLAYER_2;
    }
    if (board->bounces_right > 1 || (board->bounces_right == 1 && (ball_is_out))) {
        return PLAYER_1;
    }
    // aqui bounces == 0 para os dois
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
    board->ball_pos.x = SCREEN_WIDTH / 2;
    board->ball_pos.y = 8;
    board->ball_vel.x = get_time() & 0x1 ? V0 : -V0;
    board->ball_vel.y = 0;
    board->bounces_left = 0;
    board->bounces_right = 0;
}

void board_hit_ball(board_t *board) {
    board->ball_vel.x *= -1.1;
    if (board->ball_vel.y > 0) {
        board->ball_vel.y *= -1;
    } else {
        board->ball_vel.y *= 1.1;
    }
}

void board_update_score(board_t *board, player_t winner) {
    if (winner != PLAYER_1 && winner != PLAYER_2) {
        // ERROR
        return;
    }
    uint8_t p_idx = ((uint8_t)winner) - 1;
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
            board->score[(p_idx + 1) % 2].points = 0;
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
                    board->score[(p_idx + 1) % 2].games = 0;
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

void board_init_LCD() {
    player_t player = PLAYER_1;
    uint8_t line_offset;
    char string[5];
    uint8_t empty_LCD[] = {
        32, 32, 32, 32,
        32, 32, 32, 32,
        32, 32, 32, 32,
        32, 32, 32, 32,
        0};
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
        p_idx = player - 1;
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
        p_idx = player - 1;
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
    I2C_OLED_clrScrBuf();
    // floor
    for (i = 8; i < SCREEN_WIDTH - 8; i++) {
        for (j = FLOOR_LEVEL; j < FLOOR_LEVEL + FLOOR_HEIGHT; j++) {
            I2C_OLED_setPixel(i, j);
        }
    }
    // net
    for (i = NET_LEFT; i < NET_RIGHT; i++) {
        for (j = NET_TOP; j < FLOOR_LEVEL; j++) {
            I2C_OLED_setPixel(i, j);
        }
    }
    // ball
    if (board->ball_pos.x > 0 &&
        board->ball_pos.x <= SCREEN_WIDTH &&
        board->ball_pos.y > 0 &&
        board->ball_pos.y <= SCREEN_HEIGHT) {
        x = (uint8_t)roundf(board->ball_pos.x);
        y = (uint8_t)roundf(board->ball_pos.y);
        for (i = x - 2; i < x + 3; i++) {
            for (j = y - 2 + ABS_DIFF(i, x); j < y + 3 - ABS_DIFF(i, x); j++) {
                I2C_OLED_setPixel(i, j);
            }
        }
    }
    I2C_OLED_redisplay();
}

void board_startscreen_display(){

	// 'P' character starting at x = 0
	for (int i = 0; i < 8; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 0 || i == 1 || 
	            (i > 0 && i < 4 && j == FLOOR_LEVEL) || 
	            (i == 3 && j < FLOOR_LEVEL + 4) || 
	            (j == FLOOR_LEVEL + 4 && i < 4)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// 'R' character starting at x = 8
	for (int i = 8; i < 16; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 8 || i == 9 || 
	            (i > 8 && i < 12 && j == FLOOR_LEVEL) || 
	            (i == 11 && j < FLOOR_LEVEL + 4) || 
	            (j == FLOOR_LEVEL + 4 && i < 12) || 
	            (i > 9 && i == j - FLOOR_LEVEL + 8)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// 'E' character starting at x = 16
	for (int i = 16; i < 24; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 16 || i == 17 || 
	            (i > 16 && i < 20 && j == FLOOR_LEVEL) || 
	            (j == FLOOR_LEVEL + 4 && i < 20) || 
	            (i > 16 && i < 20 && j == FLOOR_LEVEL + 7)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// 'S' character starting at x = 24
	for (int i = 24; i < 32; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if ((i > 24 && i < 28 && (j == FLOOR_LEVEL || j == FLOOR_LEVEL + 7)) || 
	            (j == FLOOR_LEVEL + 3 && i > 24 && i < 28) || 
	            (i == 24 || i == 25) && (j < FLOOR_LEVEL + 4) || 
	            (i == 27 || i == 26) && (j >= FLOOR_LEVEL + 4)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// 'S' character starting at x = 32
	for (int i = 32; i < 40; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if ((i > 24 && i < 28 && (j == FLOOR_LEVEL || j == FLOOR_LEVEL + 7)) || 
	            (j == FLOOR_LEVEL + 3 && i > 24 && i < 28) || 
	            (i == 24 || i == 25) && (j < FLOOR_LEVEL + 4) || 
	            (i == 27 || i == 26) && (j >= FLOOR_LEVEL + 4)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// ' ' space character starting at x = 40
	// Space is usually a blank space so no pixels need to be set
	
	// 'I' character starting at x = 48
	for (int i = 56; i < 64; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 51 || i == 50) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	
	// 'R' character starting at x = 56
	for (int i = 64; i < 72; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 56 || i == 57 || 
	            (i > 56 && i < 60 && j == FLOOR_LEVEL) || 
	            (i == 59 && j < FLOOR_LEVEL + 4) || 
	            (j == FLOOR_LEVEL + 4 && i < 60) || 
	            (i > 57 && i == j - FLOOR_LEVEL + 56)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// 'Q' character starting at x = 64
	for (int i = 72; i < 80; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if ((i > 64 && i < 70 && (j == FLOOR_LEVEL || j == FLOOR_LEVEL + 7)) || 
	            (i == 64 || i == 65) && (j > FLOOR_LEVEL && j < FLOOR_LEVEL + 7) || 
	            (i == 69 || i == 68) && (j > FLOOR_LEVEL && j < FLOOR_LEVEL + 7) || 
	            (i == j - FLOOR_LEVEL + 64)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// 'A' character starting at x = 72
	for (int i = 08; i < 88; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 72 || i == 73 || 
	            (i > 72 && i < 76 && j == FLOOR_LEVEL + 4) || 
	            (i == 75 || i == 74) && (j >= FLOOR_LEVEL)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// '1' character starting at x = 80
	for (int i = 88; i < 96 i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if (i == 83 || i == 82) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
	// '2' character starting at x = 88
	for (int i = 96; i < 104; i++) {
	    for (int j = FLOOR_LEVEL; j < FLOOR_LEVEL + 8; j++) {
	        if ((i > 88 && i < 92 && (j == FLOOR_LEVEL || j == FLOOR_LEVEL + 7)) || 
	            (j == FLOOR_LEVEL + 3 && i > 88 && i < 92) || 
	            (i == 88 || i == 89) && (j < FLOOR_LEVEL + 4) || 
	            (i == 91 || i == 90) && (j >= FLOOR_LEVEL + 4)) {
	            I2C_OLED_setPixel(i, j);
	        }
	    }
	}
}
