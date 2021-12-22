#include "battle_ship.h"
//#include "Arduino.h"

#ifdef CLI_COMPILATION
#include "battle_cli.h"
#endif

#include <cstdlib>
#include <iostream>
#include <algorithm>

BattleBoard::BattleBoard(int rows, int cols, char fill_char) : rows(rows), cols(cols), total_ship_count(0), ship_counts() {
    this->_field = new char*[rows];
    this->_field[0] = new char[rows * cols];
    for (int i = 1; i < rows; i++) {
        _field[i] = _field[i - 1] + cols;
    }
    std::fill(_field[0], _field[0] + rows * cols, fill_char);

    this->_mask = new char*[rows];
    this->_mask[0] = new char[rows * cols];
    for (int i = 1; i < rows; i++) {
        _mask[i] = _mask[i - 1] + cols;
    }
    std::fill(_mask[0], _mask[0] + rows * cols, fill_char);
}

BattleBoard::~BattleBoard() {
    delete [] this->_mask[0];
    delete [] this->_mask;
    delete [] this->_field[0];
    delete [] this->_field[0];
}

SHOOT_RESULT BattleBoard::board_check_ship_destroyed(int row, int col) {
    // check vertical //1 0
    bool is_horizontal = 0;
    bool is_destroyed = 1; // true 
    int count_adjacent_ship = 0;
    for (int i = row, j = col; i < rows; i++) {
        if (_field[i][j] == '#')
            is_destroyed = 0;
        else if (_field[i][j] != 'X')
            break;
        count_adjacent_ship++;
    }
    for (int i = row - 1, j = col; i >= 0; i--) {
        if (_field[i][j] == '#')
            is_destroyed = 0;
        else if (_field[i][j] != 'X')
            break;
        count_adjacent_ship++;
    }
    // check horizontal if count less than 2    
    if (count_adjacent_ship < 2) {
        count_adjacent_ship = 0;
        is_horizontal = 1;
        for (int i = row, j = col; j < cols; j++) {
            if (_field[i][j] == '#')
                is_destroyed = 0;
            else if (_field[i][j] != 'X')
                break;
            count_adjacent_ship++;
        }
        for (int i = row, j = col - 1; j >= 0; j--) {
            if (_field[i][j] == '#')
                is_destroyed = 0;
            else if (_field[i][j] != 'X')
                break;
            count_adjacent_ship++;
        }
    }
    if (is_destroyed) {
        board_event_destroyed(row, col, is_horizontal);
        ship_counts[count_adjacent_ship]--;
        total_ship_count--;
    }
    return is_destroyed? SHOOT_RESULT::SUCCESS_FINISH_HIT : SHOOT_RESULT::SUCCESS_HIT;
}

int BattleBoard::board_set_ship( Ship sh) {
    // check up/left diagonal cells
    if (!sh.is_horizontal && sh.x - 1 >= 0) { 
        if (_field[sh.x - 1][sh.y] != ' ' ||
            (sh.y - 1 >= 0        && _field[sh.x - 1][sh.y - 1] != ' ') ||
            (sh.y + 1 < cols      && _field[sh.x - 1][sh.y + 1] != ' '))
            return -1;
    }
    else if (sh.is_horizontal && sh.y - 1 >= 0) {
        if (_field[sh.x][sh.y - 1] != ' ' ||
            (sh.x - 1 >= 0        && _field[sh.x - 1][sh.y - 1] != ' ') ||
            (sh.x + 1 < rows      && _field[sh.x + 1][sh.y - 1] != ' '))
            return -1;
    }

    for (int i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        if (sh.y >= cols || sh.x >= rows || _field[sh.x][sh.y] != ' ') {
            return -1;
        }
        if (sh.is_horizontal) {
            if (sh.x + 1 < rows && _field[sh.x + 1][sh.y] != ' ')
                return -1;
            if (sh.x - 1 >= 0 && _field[sh.x - 1][sh.y] != ' ')
                return -1;
        }
        else {
            if (sh.y + 1 < cols && _field[sh.x][sh.y + 1] != ' ')
                return -1;
            if (sh.y - 1 >= 0  && _field[sh.x][sh.y - 1] != ' ')
                return -1;
        }
    }

    // check down/right diagonal cells
    if (!sh.is_horizontal && sh.x < rows) { 
        if (_field[sh.x][sh.y] != ' ' ||
            (sh.y - 1 >= 0        && _field[sh.x][sh.y - 1] != ' ') ||
            (sh.y + 1 < cols      && _field[sh.x][sh.y + 1] != ' '))
        return -1;
    }
    else if (sh.is_horizontal && sh.y < cols) {
        if (_field[sh.x][sh.y] != ' ' ||
            (sh.x - 1 >= 0        && _field[sh.x - 1][sh.y] != ' ') ||
            (sh.x + 1 < rows      && _field[sh.x + 1][sh.y] != ' '))
            return -1;
    }

    if (sh.is_horizontal) 
        sh.y -= sh.size;
    else 
        sh.x -= sh.size;

    for (int i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        _field[sh.x][sh.y] = '#';
    }
    ship_counts[sh.size]++;
    return 0;
}

SHOOT_RESULT BattleBoard::board_fire_at(int row, int col) {
    if (row >= rows || col >= cols) {
        return SHOOT_RESULT::NOT_ALLOWED;
    }
    char cell = _field[row][col];

    switch (cell) {
    case ' ':
        _mask[row][col] = '0';
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
        for (j = col; j >= 0 && _field[row][j] == 'X'; j--);
        // fill left side 
        if (j >= 0) {
            fill_both(row, j, '0');
            if (row > 0)           fill_both(row-1, j, '0');
            if (row < rows - 1)    fill_both(row+1, j, '0');
        }
        // fill ship neigh cells
        for (j++; j < cols && _field[row][j] == 'X'; j++) {
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
        for (i = row; i >= 0 && _field[i][col] == 'X'; i--);
        // fill up side 
        if (i >= 0) {
            fill_both(i, col, '0');
            if (col > 0)           fill_both(i, col-1, '0');
            if (col < cols - 1)    fill_both(i, col+1, '0');
        }
        // fill ship neigh cells
        for (i++; i < rows && _field[i][col] == 'X'; i++) {
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

int BattleBoard::board_set_rand_ships(const std::vector<std::pair<int, int>> &ship_for_gen, long (*rand_func)() ) {
    Ship temp_ship;
    for (auto it = ship_for_gen.begin(); it < ship_for_gen.end(); it++) {
        auto ssize = it->first;
        for(int i = 0; i < it->second;) {
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


#ifdef CLI_COMPILATION
long wrap_rand() {
    return rand();
}

void game_loop(BattleBoard &brd1, PLAYER_TYPE pt1, BattleBoard &brd2, PLAYER_TYPE pt2) {
    bool setup_flag = true;
    while (setup_flag) {
        std::vector<std::pair<int,int>> counts = {{1, 4}, {2, 3}, {3, 2}, {4, 1}}; 
        // 1st player set ships  
        brd1.board_set_rand_ships(counts, wrap_rand);
        printf("Player 1 table\n");
        board_print_both(brd1);
        // 2nd player set ships
        brd2.board_set_rand_ships(counts, wrap_rand);
        printf("Player 2 table\n");
        board_print_both(brd2);
        setup_flag = false;
    }
    int winner = 0;
    int row, col;
    while (!winner) {
        // player brd1
        while(true) {
            board_print_battlefield(brd1, brd2);
            printf("Player 1 turns\n");
            if (get_coor_user_input(std::cin, brd2.get_rows(), brd2.get_cols(), &row, &col) < 0)
                continue;
            // turn 1st player
            SHOOT_RESULT status_shot = brd2.board_fire_at(row, col);
            if (status_shot == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                if (brd2.get_total_alive_count() == 0) {
                    winner = 1;
                    break;
                }
                continue;
            }
            if (status_shot != SHOOT_RESULT::MISSED_HIT) {
                // Shoot again
                continue;
            }
            break;
        }
        // turn 2nd player
        while(true) {
            board_print_battlefield(brd1, brd2);
            printf("Player 2 turns\n");
            if (get_coor_user_input(std::cin, brd1.get_rows(), brd1.get_cols(), &row, &col) < 0)
                continue;
            // turn 1st player
            SHOOT_RESULT status_shot = brd1.board_fire_at(row, col);
            if (status_shot == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                if (brd1.get_total_alive_count() == 0) {
                    winner = 2;
                    break;
                }
                continue;
            }
            if (status_shot != SHOOT_RESULT::MISSED_HIT) {
                // Shoot again
                continue;
            }
            break;
        }
    }
}
#endif