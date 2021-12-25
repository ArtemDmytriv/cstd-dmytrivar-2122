#include "battle_ship.h"
#include "battle_ship_ai.h"
//#include "Arduino.h"

#ifdef CLI_COMPILATION
#include "battle_cli.h"
#include <iostream>
#include <cstdlib>
#endif

#include <cstring>

BattleBoard::BattleBoard(int rows, int cols, char fill_char) : rows(rows), cols(cols), total_ship_count(0), ship_counts() {
    if (rows <= 0 || cols <= 0) {
#ifdef CLI_COMPILATION
        std::cerr << "Incorrect num of cols or rows";
        exit(2);
#else
        rows = 10;
        cols = 10;
#endif
    }
    this->_field = new char[rows * cols];
    memset(_field, fill_char, rows*cols);
    this->_mask = new char[rows * cols];
    memset(_mask, fill_char, rows*cols);
}

BattleBoard::~BattleBoard() {
    delete [] this->_mask;
    delete [] this->_field;
}

SHOOT_RESULT BattleBoard::board_check_ship_destroyed(int row, int col) {
    // check vertical //1 0
    bool is_horizontal = 0;
    bool is_destroyed = 1; // true 
    int count_adjacent_ship = 0;
    for (int i = row, j = col; i < rows; i++) {
        if (field(i, j) == '#')
            is_destroyed = 0;
        else if (field(i, j) != 'X')
            break;
        count_adjacent_ship++;
    }
    for (int i = row - 1, j = col; i >= 0; i--) {
        if (field(i, j) == '#')
            is_destroyed = 0;
        else if (field(i, j) != 'X')
            break;
        count_adjacent_ship++;
    }
    // check horizontal if count less than 2    
    if (count_adjacent_ship < 2) {
        count_adjacent_ship = 0;
        is_horizontal = 1;
        for (int i = row, j = col; j < cols; j++) {
            if (field(i, j) == '#')
                is_destroyed = 0;
            else if (field(i, j) != 'X')
                break;
            count_adjacent_ship++;
        }
        for (int i = row, j = col - 1; j >= 0; j--) {
            if (field(i, j) == '#')
                is_destroyed = 0;
            else if (field(i, j) != 'X')
                break;
            count_adjacent_ship++;
        }
    }
    if (is_destroyed) {
        board_event_destroyed(row, col, is_horizontal);
        ship_counts[count_adjacent_ship - 1]--;
        total_ship_count--;
    }
    return is_destroyed? SHOOT_RESULT::SUCCESS_FINISH_HIT : SHOOT_RESULT::SUCCESS_HIT;
}

int BattleBoard::board_set_ship( Ship sh) {
    // check up/left diagonal cells
    if (!sh.is_horizontal && sh.x - 1 >= 0) { 
        if (field(sh.x - 1, sh.y) != ' ' ||
            (sh.y - 1 >= 0        && field(sh.x - 1, sh.y - 1) != ' ') ||
            (sh.y + 1 < cols      && field(sh.x - 1, sh.y + 1) != ' '))
            return -1;
    }
    else if (sh.is_horizontal && sh.y - 1 >= 0) {
        if (field(sh.x, sh.y - 1) != ' ' ||
            (sh.x - 1 >= 0        && field(sh.x - 1, sh.y - 1) != ' ') ||
            (sh.x + 1 < rows      && field(sh.x + 1, sh.y - 1) != ' '))
            return -1;
    }

    for (int i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        if (sh.y >= cols || sh.x >= rows || field(sh.x, sh.y) != ' ') {
            return -1;
        }
        if (sh.is_horizontal) {
            if (sh.x + 1 < rows && field(sh.x + 1, sh.y) != ' ')
                return -1;
            if (sh.x - 1 >= 0 && field(sh.x - 1, sh.y) != ' ')
                return -1;
        }
        else {
            if (sh.y + 1 < cols && field(sh.x, sh.y + 1) != ' ')
                return -1;
            if (sh.y - 1 >= 0  && field(sh.x, sh.y - 1) != ' ')
                return -1;
        }
    }

    // check down/right diagonal cells
    if (!sh.is_horizontal && sh.x < rows) { 
        if (field(sh.x, sh.y) != ' ' ||
            (sh.y - 1 >= 0        && field(sh.x, sh.y - 1) != ' ') ||
            (sh.y + 1 < cols      && field(sh.x, sh.y + 1) != ' '))
        return -1;
    }
    else if (sh.is_horizontal && sh.y < cols) {
        if (field(sh.x, sh.y) != ' ' ||
            (sh.x - 1 >= 0        && field(sh.x - 1, sh.y) != ' ') ||
            (sh.x + 1 < rows      && field(sh.x + 1, sh.y) != ' '))
            return -1;
    }

    if (sh.is_horizontal) 
        sh.y -= sh.size;
    else 
        sh.x -= sh.size;

    for (int i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        field(sh.x, sh.y) = '#';
    }
    ship_counts[sh.size - 1]++;
    return 0;
}

SHOOT_RESULT BattleBoard::board_fire_at(int row, int col) {
    if (row >= this->rows || col >= this->cols) {
        return SHOOT_RESULT::NOT_ALLOWED;
    }
    char cell = field(row, col);

    switch (cell) {
    case ' ':
        fill_both(row, col, '0');
        return SHOOT_RESULT::MISSED_HIT;
    case '0':
    case 'X':
        return SHOOT_RESULT::NOT_ALLOWED;
    case '#':
        fill_both(row, col, 'X');
        return board_check_ship_destroyed(row, col);
    } 
    return SHOOT_RESULT::NOT_ALLOWED;
}

void BattleBoard::board_event_destroyed(int row, int col, bool is_horizontal) {
    if (is_horizontal) { //1 0
        int j = 0;
        // find left corner
        for (j = col; j >= 0 && field(row, j) == 'X'; j--);
        // fill left side 
        if (j >= 0) {
            fill_both(row, j, '0');
            if (row > 0)           fill_both(row-1, j, '0');
            if (row < rows - 1)    fill_both(row+1, j, '0');
        }
        // fill ship neigh cells
        for (j++; j < cols && field(row, j) == 'X'; j++) {
            if (row > 0)           fill_both(row-1, j, '0');
            if (row < rows - 1)    fill_both(row+1, j, '0');
        }
        // fill right side
        if (j < cols) {
            fill_both(row, j, '0');
            if (row > 0)           fill_both(row-1, j, '0');
            if (row < rows - 1)    fill_both(row+1, j, '0');
        }
    }
    else {
        int i = 0;
        // find up corner
        for (i = row; i >= 0 && field(i, col) == 'X'; i--);
        // fill up side 
        if (i >= 0) {
            fill_both(i, col, '0');
            if (col > 0)           fill_both(i, col-1, '0');
            if (col < cols - 1)    fill_both(i, col+1, '0');
        }
        // fill ship neigh cells
        for (i++; i < rows && field(i, col) == 'X'; i++) {
            if (col > 0)           fill_both(i, col-1, '0');
            if (col < cols - 1)    fill_both(i, col+1, '0');
        }
        // fill down side
        if (i < rows) {
            fill_both(i, col, '0');
            if (col > 0)           fill_both(i, col-1, '0');
            if (col < cols - 1)    fill_both(i, col+1, '0');
        }
    }
}

void BattleBoard::clear(char fill_char) {
    memset(_field, fill_char, rows*cols);
    memset(_mask, fill_char, rows*cols);
}

int BattleBoard::board_set_rand_ships(const int ship_for_gen[4], long (*rand_func)() ) {
    this->clear();
    Ship temp_ship;
    for (int j = 0; j < 4; j++) {
        auto ssize = j + 1;
        for(int i = 0; i < ship_for_gen[j];) {
            temp_ship.x = rand_func() % rows;
            temp_ship.y = rand_func() % cols;
            temp_ship.size = ssize;
            temp_ship.is_horizontal = rand_func() % 2;
            if (board_set_ship(temp_ship) == 0) {  
                this->total_ship_count++;
                i++;
            }
        }
    }
    return 0;
}
