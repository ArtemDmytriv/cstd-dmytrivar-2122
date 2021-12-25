#include "battle_ship_ai.h"
#include <utility>

AI_BattleShip::AI_BattleShip(unsigned char *buffer, long (*rand_f)()) : row(-1), col(-1), ship_direct(-1), 
    need_to_finish_ship(false), rand_func(rand_f), avail_moves(buffer), avail_moves_count(0) {}

AI_BattleShip::~AI_BattleShip() {
}

static inline void swap(unsigned char *val1, unsigned char *val2) {
    int temp = *val1;
    *val1 = *val2;
    *val2 = temp;
} 
SHOOT_RESULT AI_BattleShip::rand_fire(BattleBoard *enemy_brd) {
    int pos = rand_func() % this->avail_moves_count;
    char buffer[128];
    this->row = avail_moves[pos] / 10;
    this->col = avail_moves[pos] % 10;
    swap( &this->avail_moves[pos], &this->avail_moves[--this->avail_moves_count] );

    return enemy_brd->board_fire_at(row, col);
}
SHOOT_RESULT AI_BattleShip::continue_fire(BattleBoard *enemy_brd) {
    SHOOT_RESULT result = SHOOT_RESULT::MISSED_HIT;
    if (ship_direct < 0) { 
        int choose_dir = rand_func() % 4;
        int copy_row = row;
        int copy_col = col;

        switch(choose_dir) {
            case 0: // up 
                if (row + 1 < enemy_brd->get_rows() && enemy_brd->mask(row + 1, col) != '0') {
                    result = enemy_brd->board_fire_at(row + 1, col);
                    choose_dir = 0; break;
                }
            case 1: // down
                if (row - 1 >= 0 && enemy_brd->mask(row - 1, col) != '0') {
                    result = enemy_brd->board_fire_at(row - 1, col); 
                    choose_dir = 1; break;
                }
            case 2: // right
                if (col + 1 < enemy_brd->get_cols() && enemy_brd->mask(row, col + 1) != '0') {
                    result = enemy_brd->board_fire_at(row, col + 1);
                    choose_dir = 2; break;
                }
            case 3: // left
                if (col - 1 >=0 && enemy_brd->mask(row, col - 1) != '0') {
                    result = enemy_brd->board_fire_at(row, col - 1);
                    choose_dir = 3; break;
                }   
        }
        if (result == SHOOT_RESULT::MISSED_HIT) { // restore col and row when missed
            col = copy_col;
            row = copy_row;
        }
        else { // else set ship direct
            ship_direct = choose_dir < 2? 1 : 0;
        }
    }
    else if (ship_direct == 0) { // horizontal ship hidden
        int j = 0;
        for(j = col; j >= 0 && enemy_brd->mask(row, j) == 'X'; j--);
        if (j >= 0 && enemy_brd->mask(row, j) == ' ') {
            return enemy_brd->board_fire_at(row, j);
        }
        // find lower edge
        for(j = col; j < enemy_brd->get_cols() && enemy_brd->mask(row, j) == 'X'; j++);
        if (j < enemy_brd->get_cols() && enemy_brd->mask(row, j) == ' ') {
            return enemy_brd->board_fire_at(row, j);
        }
    } 
    else { // vertical ship hidden
        // find upper edge
        int i = 0;
        for(i = row; i >= 0 && enemy_brd->mask(i, col) == 'X'; i--);
        if (i >= 0 && enemy_brd->mask(i, col) == ' ') {
            return enemy_brd->board_fire_at(i, col);
        }
        // find lower edge
        for(i = row; i < enemy_brd->get_rows() && enemy_brd->mask(i, col) == 'X'; i++);
        if (i < enemy_brd->get_rows() && enemy_brd->mask(i, col) == ' ') {
            return enemy_brd->board_fire_at(i, col);
        }
    }
    return result;
}

SHOOT_RESULT AI_BattleShip::fire(BattleBoard *enemy_brd) {
    SHOOT_RESULT res;
    if (this->need_to_finish_ship) {
        res = this->continue_fire(enemy_brd);
    }
    else
        res = this->rand_fire(enemy_brd);

    if (res == SHOOT_RESULT::SUCCESS_HIT) {
        this->need_to_finish_ship = true;
    }
    else if (res == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
        this->reset_vars();
        this->on_destroy_event(enemy_brd);
    }
    return res;
}   

void AI_BattleShip::on_destroy_event(BattleBoard *enemy_brd) {
    // check for filled cells
    for(int pos = 0; pos < avail_moves_count; pos++) {
        if (enemy_brd->mask(avail_moves[pos] / enemy_brd->get_rows(), avail_moves[pos] % enemy_brd->get_cols()) == '0') {
            swap( &avail_moves[pos], &avail_moves[--avail_moves_count] );
        }
    }
}

void AI_BattleShip::init_moves(int enemy_rows, int enemy_cols) {
    this->avail_moves_count = enemy_rows * enemy_cols;
    for (int i = 0; i < avail_moves_count; i++) {
        this->avail_moves[i] = i;
    }
}

void AI_BattleShip::reset_vars() {
    this->row = -1;
    this->col = -1;
    this->ship_direct = -1;
    this->need_to_finish_ship = false;
}