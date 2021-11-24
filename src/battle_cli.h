#ifndef _BATTLE_CLI_H
#define _BATTLE_CLI_H

#define SHIP_CELL "###"
#define SHOOT_SHIP_CELL "+X+"
#define EMPTY_CELL "   "
#define MISSED_CELL " O "

#include "battle_ship.h"

void board_print(battle_board *brd);

#endif // _BATTLE_CLI_H