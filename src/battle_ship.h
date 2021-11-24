#ifndef _BATTLE_SHIP_H
#define _BATTLE_SHIP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    FIELD_ENUM,
    MASK_ENUM
} BOARD_TYPE;

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
    size_t size;
    size_t ship_count;
} battle_board;

typedef struct {
    size_t x, y;
    size_t size;
    bool is_horizontal;
} ship;

int board_init(battle_board *brd, size_t size, char fill);
int board_deinit(battle_board *brd);

int board_set_ship(battle_board *brd, ship sh);
int board_set_rand_ships(battle_board *brd);

// watch on ships on field and fill mask with result of shoot
SHOOT_RESULT board_fire_at(battle_board *brd, size_t row, size_t col);
SHOOT_RESULT board_check_ship_destroyed(battle_board *brd, size_t row, size_t col);

void board_print(battle_board *brd); //, BOARD_TYPE ftype);

#endif