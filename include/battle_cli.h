#ifndef _BATTLE_CLI_H
#define _BATTLE_CLI_H

#define SHIP_CELL "###"
#define SHOOT_SHIP_CELL "+X+"
#define EMPTY_CELL "   "
#define MISSED_CELL " O "

#include "battle_ship.h"
#include <cstddef>
#include <iosfwd>

// print both "field" and "mask" of one player board
void board_print_both(const BattleBoard &brd);

// hide ships, print only "mask" fields of both players
void board_print_battlefield(const BattleBoard &brd1, const BattleBoard &brd2);

int get_coor_user_input(std::istream &in, int max_row, int max_col, int *row, int *col);

#endif // _BATTLE_CLI_H