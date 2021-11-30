#ifndef _BATTLE_SHIP_H
#define _BATTLE_SHIP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SHIP_COUNT_POS 0
#define SHIP_MAX_SIZE 4
#define SHIP_MIN_SIZE 1

typedef enum {
    HUMAN_PLAYER_TYPE,
    AI_PLAYER_TYPE
} PLAYER_TYPE;

typedef enum {
    NOT_ALLOWED,
    MISSED_HIT,
    SUCCESS_HIT,
    SUCCESS_FINISH_HIT
} SHOOT_RESULT;

typedef struct {
    // represents 2d square array of field, board with own ships
    char **field;
    // represents 2d square array of field, will shared to the opponent
    char **mask;
    // number row/col of the board (size x size)
    uint8_t size;
    uint8_t ship_counts[SHIP_MAX_SIZE + 1]; // total_ships, 1, 2, 3, 4;
} battle_board;

typedef struct {
    uint8_t x, y;
    uint8_t size;
    bool is_horizontal;
} ship;

int board_init(battle_board *brd, uint8_t size, char fill);
int board_deinit(battle_board *brd);

int board_set_ship(battle_board *brd, ship sh);
int board_set_rand_ships(battle_board *brd, const uint8_t *ship_gen_counts);

// watch on ships on field and fill mask with result of shoot
SHOOT_RESULT board_fire_at(battle_board *brd, uint8_t row, uint8_t col);
SHOOT_RESULT board_check_ship_destroyed(battle_board *brd, uint8_t row, uint8_t col);
void board_event_destroyed(battle_board *brd, uint8_t row, uint8_t col, uint8_t is_horizontal);

void game_loop(battle_board *brd1, PLAYER_TYPE pt1, battle_board *brd2, PLAYER_TYPE pt2);

#endif