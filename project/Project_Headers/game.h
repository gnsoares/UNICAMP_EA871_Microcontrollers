/**
 * @file mcu.h
 * @author Gustavo Nascimento Soares
 * @author João Pedro Souza Pascon
 * @brief Prototipos, macros e tipos de dados de game
 * @date 2024-06-15
 */

#ifndef _GAME_H
#define _GAME_H

#include <stdint.h>

typedef enum {
    PLAYER_NONE,
    PLAYER_1,
    PLAYER_2
} player_t;

typedef enum {
    LEFT,
    MIDDLE,
    RIGHT
} region_t;

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
    uint8_t bounces_left;
    uint8_t bounces_right;
} board_t;

/**
 * @brief Loop de execucao do jogo
 *
 * @param[in] sets_to_win
 * @noreturn
 */
void game_loop(uint8_t sets_to_win);
/**
 * @brief Mostra no OLED um padrao de zadrez
 *
 * Mostra apenas fora da regiao reservada para informacoes
 *
 */
void game_display_checkerboard(void);
/**
 * @brief Mostra no OLED na regiao reservada para informacoes como iniciar a partida
 *
 */
void board_start_screen_display(void);
/**
 * @brief Mostra no OLED na regiao reservada para informacoes o jogador vencedor
 *
 * @param[in] winner jogador vencedor da partida
 */
void game_winner_screen_display(player_t winner);

/**
 * @brief Reinicia a partida
 *
 * @param[in,out] board estrutura do estado da partida
 */
void board_reset(board_t *board);
/**
 * @brief Atualiza o estado da partida
 *
 * Faz os calculos da fisica dos elementos do jogo
 *
 * @param[in,out] board estrutura do estado da partida
 * @param[in] dt diferenca de tempo em milissegundos desde a ultima execucao
 */
void board_update(board_t *board, uint32_t dt);
/**
 * @brief Verifica se algum jogador venceu o ponto
 *
 * @param[in,out] board estrutura do estado da partida
 * @return jogador que venceu o ponto atual ou PLAYER_NONE caso nao haja vencedor
 */
player_t board_check_winner_point(board_t *board);
/**
 * @brief Verifica se algum jogador venceu a partida
 *
 * @param[in,out] board estrutura do estado da partida
 * @param[in] sets_to_win
 * @return jogador que venceu a partida ou PLAYER_NONE caso nao haja vencedor
 */
player_t board_check_winner_match(board_t *board, uint8_t sets_to_win);
/**
 * @brief Reposiciona a bola no meio da quadra e da uma velocidade inicial aleatoria
 *
 * @param[in,out] board estrutura do estado da partida
 */
void board_reset_ball(board_t *board);
/**
 * @brief Registra um rebatimento da bola e atualiza sua velocidade
 *
 * @param[in,out] board estrutura do estado da partida
 */
void board_hit_ball(board_t *board);
/**
 * @brief Atualiza o placar da partida e o LCD
 *
 * @param[in,out] board estrutura do estado da partida
 * @param[in] winner vencedor do ponto
 * @param[in] games_to_set quantos games necessarios para vencer set
 */
void board_update_score(board_t *board, player_t winner, uint8_t games_to_set);
/**
 * @brief Mostra no LCD um placar vazio
 *
 */
void board_init_LCD();
/**
 * @brief Atualiza no LCD a contagem de games para cada jogador
 *
 * @param[in,out] board estrutura do estado da partida
 */
void board_update_LCD_games(board_t *board);
/**
 * @brief Atualiza no LCD a contagem de pontos para cada jogador
 *
 * @param[in,out] board estrutura do estado da partida
 */
void board_update_LCD_points(board_t *board);
/**
 * @brief Mostra no OLED a visualizacao do estado atual da partida
 *
 * @param[in,out] board estrutura do estado da partida
 */
void board_display(board_t *board);

#endif
