/**
 * @file game.c
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief Funcoes de controle do fluxo do jogo
 * @date 2024-06-15
 */

#include "game.h"

#include <math.h>

#include "ISR.h"
#include "mcu.h"

// valor absoluto da diferenca de dois valores
#define ABS_DIFF(a, b) ((a) > (b) ? ((a) - (b)) : ((b) - (a)))

// constantes fisicas
#define PIXELS_P_METER 4.712                   // 112 pixels / 23.77 m
#define G 9.81 * PIXELS_P_METER / 1000 / 1000  // pixels / ms^2
#define V0 5 * PIXELS_P_METER / 1000           // pixels / ms

// dimensoes da quadra
#define NET_TOP SCREEN_HEIGHT - 24
#define NET_LEFT SCREEN_WIDTH / 2 - 1
#define NET_RIGHT SCREEN_WIDTH / 2 + 1
#define FLOOR_LEVEL SCREEN_HEIGHT - 6
#define FLOOR_HEIGHT 2

// dimensoes do retangulo onde info sao mostradas nas telas de inicio e ganhador
#define WAIT_SCREEN_INNER_RECT_XMIN 24
#define WAIT_SCREEN_INNER_RECT_XMAX 104
#define WAIT_SCREEN_INNER_RECT_YMIN 16
#define WAIT_SCREEN_INNER_RECT_YMAX 48

void game_loop(uint8_t sets_to_win) {
    player_t winner_match = PLAYER_NONE, winner_point = PLAYER_NONE;
    board_t *board = ISR_getBoard();
    uint32_t t1, t2;  // ms
    ISR_setState(PREPARA_INICIO);

    while (1) {
        switch (ISR_getState()) {
            case PREPARA_INICIO:
                board_reset(board);
                GPIO_switches_IRQAn_interrupt_ativa(12, BTN_IRQC);
                ISR_setState(INICIO);
                board_init_LCD(board);
                board_start_screen_display();
                break;
            case INICIO:
                game_display_checkerboard();
                break;
            case LAUNCH_BALL:
                board_reset_ball(board);
                ISR_setState(PLAYER_TURN);
                reset_time();
                t1 = get_time();
                if (board->ball_vel.x < 0) {
                    // bola foi para a esquerda: jogador 1 deve rebater
                    GPIO_switches_IRQAn_interrupt_ativa(4, BTN_IRQC);
                    ISR_setPlayer(PLAYER_1);
                } else {
                    // bola foi para a direita: jogador 2 deve rebater
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
                    // ponto ainda nao acabou: nada a fazer
                    ISR_setState(PLAYER_TURN);
                    break;
                }
                // jogador venceu ponto: desabilitar botoes
                GPIO_switches_IRQAn_interrupt_desativa(4);
                GPIO_switches_IRQAn_interrupt_desativa(5);

                board_update_score(board, winner_point, 1);
                winner_match = board_check_winner_match(board, sets_to_win);
                ISR_setState(winner_match != PLAYER_NONE ? WIN_SCREEN : LAUNCH_BALL);
                break;
            case WIN_SCREEN:
                game_winner_screen_display(winner_match);
                ISR_setState(WIN_VISU);
                reset_time();
                t1 = get_time();
            case WIN_VISU:
                // 5s para visualizacao
                while (get_time() - t1 < 5000) {
                    game_display_checkerboard();
                }
                ISR_setState(PREPARA_INICIO);
                break;
            default:
                break;
        }
    }
}

void game_display_checkerboard(void) {
    static uint8_t toggle = 0;
    uint8_t i, j;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        for (j = 0; j < SCREEN_HEIGHT; j++) {
            // nao fazer nada na regiao de informacoes
            if (i >= WAIT_SCREEN_INNER_RECT_XMIN && i < WAIT_SCREEN_INNER_RECT_XMAX && j >= WAIT_SCREEN_INNER_RECT_YMIN && j < WAIT_SCREEN_INNER_RECT_YMAX) {
                continue;
            }
            // quadrados de 8x8
            if ((i / 8 + j / 8) % 2 == toggle) {
                I2C_OLED_setPixel(i, j);
            } else {
                I2C_OLED_clrPixel(i, j);
            }
        }
    }
    // inverte quadrados
    toggle = (toggle + 1) % 2;

    I2C_OLED_redisplay();
    espera_2us(250000);
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
    // IMPROV: take into consideration court dimensions
    uint8_t ball_is_out = board->ball_pos.x < 0 || board->ball_pos.x >= SCREEN_WIDTH;
    if (board->bounces_left > 1 || (board->bounces_left == 1 && (ball_is_out))) {
        return PLAYER_2;
    }
    if (board->bounces_right > 1 || (board->bounces_right == 1 && (ball_is_out))) {
        return PLAYER_1;
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
    board->ball_pos.x = SCREEN_WIDTH / 2;
    board->ball_pos.y = 8;
    board->ball_vel.x = get_time() & 0x1 ? V0 : -V0;
    board->ball_vel.y = 0;
    board->bounces_left = 0;
    board->bounces_right = 0;
}

void board_hit_ball(board_t *board) {
    // velocidade horizontal sempre aumenta
    board->ball_vel.x *= -1.1;
    if (board->ball_vel.y > 0) {
        // bola caindo: reflete velocidade vertical
        board->ball_vel.y *= -1;
    } else {
        // bola subindo: bola fica mais rapida
        board->ball_vel.y *= 1.1;
    }
}

void board_update_score(board_t *board, player_t winner, uint8_t games_to_set) {
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
            // player venceu game

            // reseta pontos para proximo game
            board->score[p_idx].points = 0;
            board->score[(p_idx + 1) % 2].points = 0;

            // verifica se player ganhou set
            if (board->score[p_idx].games == games_to_set - 1) {
                // IMPROV: 2+ games diff
                board->score[p_idx].games += 1;
                board_update_LCD_games(board);
                board->score[p_idx].games = 0;
                board->score[(p_idx + 1) % 2].games = 0;
                board->score[p_idx].sets += 1;
            } else {
                board->score[p_idx].games += 1;
                board_update_LCD_games(board);
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

void board_start_screen_display() {
    uint8_t i, j, top = WAIT_SCREEN_INNER_RECT_YMIN + 6, left = WAIT_SCREEN_INNER_RECT_XMIN + 12;
    I2C_OLED_clrScrBuf();

    // PRESS
    I2C_OLED_write_P(left, top);
    I2C_OLED_write_R(left + 8, top);
    I2C_OLED_write_E(left + 16, top);
    I2C_OLED_write_S(left + 24, top);
    I2C_OLED_write_S(left + 32, top);

    // Update top position for the next line
    top += 12;
    left += 8;

    // IRQA12
    I2C_OLED_write_I(left, top);
    I2C_OLED_write_R(left + 8, top);
    I2C_OLED_write_Q(left + 16, top);
    I2C_OLED_write_A(left + 26, top);
    I2C_OLED_write_I(left + 34, top);
    I2C_OLED_write_2(left + 42, top);

    I2C_OLED_redisplay();
}

void game_winner_screen_display(player_t winner) {
    uint8_t i, j, top = WAIT_SCREEN_INNER_RECT_YMIN + 6, left = WAIT_SCREEN_INNER_RECT_XMIN + 12;
    I2C_OLED_clrScrBuf();

    // P1 / P2
    I2C_OLED_write_P(left, top);
    if (winner == PLAYER_2) {
        I2C_OLED_write_2(left + 8, top);
    } else if (winner == PLAYER_1) {
        I2C_OLED_write_I(left + 8, top);
    }

    // Update top position for the next line
    top += 12;
    left += 8;

    // WINS
    I2C_OLED_write_W(left, top);
    I2C_OLED_write_I(left + 8, top);
    I2C_OLED_write_N(left + 16, top);
    I2C_OLED_write_S(left + 26, top);

    I2C_OLED_redisplay();
}
