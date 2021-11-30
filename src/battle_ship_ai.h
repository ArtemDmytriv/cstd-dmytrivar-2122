#ifndef _BATTLE_AI_H
#define _BATTLE_AI_H

#include "battle_ship.h"

typedef struct AI_params{
    int prev_row;
    int prev_col;
    int ship_direct;
    bool need_to_finish_ship;
} AI_params;


SHOOT_RESULT battle_AI_handler(battle_board *brd);
SHOOT_RESULT battle_AI_rand_fire(battle_board *brd, int *prefered_moves, int *moves_count, AI_params *aip);
SHOOT_RESULT battle_AI_continue_fire(battle_board *brd, AI_params *aip);

#endif