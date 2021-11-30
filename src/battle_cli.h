#ifndef _BATTLE_CLI_H
#define _BATTLE_CLI_H

#define SHIP_CELL "###"
#define SHOOT_SHIP_CELL "+X+"
#define EMPTY_CELL "   "
#define MISSED_CELL " O "

#include <stdio.h>
#include "battle_ship.h"

// print both "field" and "mask" of one player board
void board_print_both(battle_board *brd);

// hide ships, print only "mask" fields of both players
void board_print_battlefield(battle_board *brd1, battle_board *brd2);

int get_coor_user_input(FILE *in, uint8_t size, uint8_t *row, uint8_t *col);

#endif // _BATTLE_CLI_H