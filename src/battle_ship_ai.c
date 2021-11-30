#include "battle_ship_ai.h"
#include <stdlib.h>

static inline void swap(int *val1, int *val2) {
    int temp = *val1;
    *val1 = *val2;
    *val2 = temp;
} 

SHOOT_RESULT battle_AI_handler(battle_board *brd) {
    return MISSED_HIT;
}

SHOOT_RESULT battle_AI_rand_fire(battle_board *brd, int *prefered_moves, int *moves_count, AI_params *aip) {
    int pos = rand() % *moves_count;
    aip->prev_row = prefered_moves[pos] / brd->size;
    aip->prev_col = prefered_moves[pos] % brd->size;
    swap(prefered_moves + pos, prefered_moves + (--*moves_count));
    return board_fire_at(brd, aip->prev_row, aip->prev_col);
}

SHOOT_RESULT battle_AI_continue_fire(battle_board *brd, AI_params *aip) {
    SHOOT_RESULT result = MISSED_HIT;
    if (aip->ship_direct < 0) { 
        int choose_dir = rand() % 4;
        int copy_row = aip->prev_row;
        int copy_col = aip->prev_col;

        switch(choose_dir) {
            case 0: // up 
                if (aip->prev_row + 1 < brd->size && brd->mask[aip->prev_row + 1][aip->prev_col] != '0') {
                    result = board_fire_at(brd, aip->prev_row + 1, aip->prev_col); break;
                }
            case 1: // down
                if (aip->prev_row - 1 >= 0 && brd->mask[aip->prev_row - 1][aip->prev_col] != '0') {
                    result = board_fire_at(brd, aip->prev_row - 1, aip->prev_col); break;
                }
            case 2: // right
                if (aip->prev_col + 1 < brd->size && brd->mask[aip->prev_row][aip->prev_col + 1] != '0') {
                    result = board_fire_at(brd, aip->prev_row, aip->prev_col + 1); break;
                }
            case 3: // left
                if (aip->prev_col - 1 >=0 && brd->mask[aip->prev_row][aip->prev_col - 1] != '0') {
                    result = board_fire_at(brd, aip->prev_row, aip->prev_col - 1); break;
                }   
        }
        if (result == MISSED_HIT) { // restore prev_col and prev_row when missed
            aip->prev_col = copy_col;
            aip->prev_row = copy_row;
        }
        else {
            aip->ship_direct = choose_dir < 2? 0 : 1;
        }
    }
    else if (aip->ship_direct == 0) { // horizontal ship hidden
        int8_t j = 0;
        for(j = aip->prev_col; j >= 0 && brd->mask[aip->prev_row][j] == 'X'; j--);
        if (j >= 0 && brd->mask[aip->prev_row][j] == ' ') {
            return board_fire_at(brd, aip->prev_row, j);
        }
        // find lower edge
        for(j = aip->prev_col; j < brd->size && brd->mask[aip->prev_row][j] == 'X'; j++);
        if (j < brd->size && brd->mask[aip->prev_row][aip->prev_col] == ' ') {
            return board_fire_at(brd, aip->prev_row, j);
        }
    } 
    else { // vertical ship hidden
        // find upper edge
        int8_t i = 0;
        for(i = aip->prev_row; i >= 0 && brd->mask[i][aip->prev_col] == 'X'; i--);
        if (i >= 0 && brd->mask[i][aip->prev_col] == ' ') {
            return board_fire_at(brd, i, aip->prev_col);
        }
        // find lower edge
        for(i = aip->prev_row; i < brd->size && brd->mask[i][aip->prev_col] == 'X'; i++);
        if (i < brd->size && brd->mask[i][aip->prev_col] == ' ') {
            return board_fire_at(brd, i, aip->prev_col);
        }
    }
    return result;
}