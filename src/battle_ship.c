#include "battle_ship.h"
#include "battle_cli.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

static inline void fill_both_arr(battle_board *brd, uint8_t row, uint8_t col, char val) {
    brd->mask[row][col] = val;
    brd->field[row][col] = val;
}

int board_init(battle_board *brd, uint8_t size, char fill) {
    brd->size = size;
    memset(brd->ship_counts, 0, sizeof(*brd->ship_counts) * (SHIP_MAX_SIZE + 1));
    size_t new_2d_size = size * sizeof(char**) + size * size; 

    brd->field = (char**)malloc(new_2d_size);
    if (!brd->field)
        return EXIT_FAILURE;
    for(uint8_t i = 0; i < size; i++) {
        brd->field[i] = (char*)(brd->field + size) + size * i;
    }
    memset(brd->field[0], fill, size * size);

    brd->mask = (char**)malloc(new_2d_size);
    if (!brd->mask)
        return EXIT_FAILURE;
    for(uint8_t i = 0; i < size; i++)
        brd->mask[i] = (char*)(brd->mask + size) + size * i;
    memset(brd->mask[0], fill, size * size);

    return EXIT_SUCCESS;
}

int board_deinit(battle_board *brd) {
    free(brd->field);
    free(brd->mask);
    brd->field = NULL;
    brd->mask = NULL;

    return EXIT_SUCCESS;
}

SHOOT_RESULT board_check_ship_destroyed(battle_board *brd, uint8_t row, uint8_t col) {
    // check vertical
    uint8_t is_horizontal = 0;
    uint8_t is_destroyed = 1; // true 
    uint8_t count_adjacent_ship = 0;
    for (uint8_t i = row, j = col; i < brd->size; i++) {
        if (brd->field[i][j] == '#')
            is_destroyed = 0;
        else if (brd->field[i][j] != 'X')
            break;
        count_adjacent_ship++;
    }
    for (int8_t i = row - 1, j = col; i >= 0; i--) {
        if (brd->field[i][j] == '#')
            is_destroyed = 0;
        else if (brd->field[i][j] != 'X')
            break;
        count_adjacent_ship++;
    }
    // check horizontal if count less than 2    
    if (count_adjacent_ship < 2) {
        count_adjacent_ship = 0;
        is_horizontal = 1;
        for (uint8_t i = row, j = col; i < brd->size; j++) {
            if (brd->field[i][j] == '#')
                is_destroyed = 0;
            else if (brd->field[i][j] != 'X')
                break;
            count_adjacent_ship++;
        }
        for (int8_t i = row, j = col - 1; i >= 0; j--) {
            if (brd->field[i][j] == '#')
                is_destroyed = 0;
            else if (brd->field[i][j] != 'X')
                break;
            count_adjacent_ship++;
        }
    }

    printf("Ship Size = %d\n", count_adjacent_ship);
    if (is_destroyed) {
        board_event_destroyed(brd, row, col, is_horizontal);
        brd->ship_counts[count_adjacent_ship]--;
        brd->ship_counts[SHIP_COUNT_POS]--;
    }
    return is_destroyed? SUCCESS_FINISH_HIT : SUCCESS_HIT;
}

int board_set_ship(battle_board *brd, ship sh) {
    // check up/left diagonal cells
    if (!sh.is_horizontal && sh.x - 1 >= 0) { 
        if (brd->field[sh.x - 1][sh.y] != ' ' ||
            (sh.y - 1 >= 0        && brd->field[sh.x - 1][sh.y - 1] != ' ') ||
            (sh.y + 1 < brd->size && brd->field[sh.x - 1][sh.y + 1] != ' '))
            return -1;
    }
    else if (sh.is_horizontal && sh.y - 1 >= 0) {
        if (brd->field[sh.x][sh.y - 1] != ' ' ||
            (sh.x - 1 >= 0        && brd->field[sh.x - 1][sh.y - 1] != ' ') ||
            (sh.x + 1 < brd->size && brd->field[sh.x + 1][sh.y - 1] != ' '))
            return -1;
    }

    for (uint8_t i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        if (sh.y >= brd->size || sh.x >= brd->size || brd->field[sh.x][sh.y] != ' ') {
            return -1;
        }
        if (sh.is_horizontal) {
            if (sh.x + 1 < brd->size && brd->field[sh.x + 1][sh.y] != ' ')
                return -1;
            if (sh.x - 1 >= 0 && brd->field[sh.x - 1][sh.y] != ' ')
                return -1;
        }
        else {
            if (sh.y + 1 < brd->size && brd->field[sh.x][sh.y + 1] != ' ')
                return -1;
            if (sh.y - 1 >= 0  && brd->field[sh.x][sh.y - 1] != ' ')
                return -1;
        }
    }

    // check down/right diagonal cells
    if (!sh.is_horizontal && sh.x < brd->size) { 
        if (brd->field[sh.x][sh.y] != ' ' ||
            (sh.y - 1 >= 0        && brd->field[sh.x][sh.y - 1] != ' ') ||
            (sh.y + 1 < brd->size && brd->field[sh.x][sh.y + 1] != ' '))
        return -1;
    }
    else if (sh.is_horizontal && sh.y < brd->size) {
        if (brd->field[sh.x][sh.y] != ' ' ||
            (sh.x - 1 >= 0        && brd->field[sh.x - 1][sh.y] != ' ') ||
            (sh.x + 1 < brd->size && brd->field[sh.x + 1][sh.y] != ' '))
            return -1;
    }

    if (sh.is_horizontal) sh.y -= sh.size;
    else sh.x -= sh.size;

    for (uint8_t i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        brd->field[sh.x][sh.y] = '#';
    }
    brd->ship_counts[sh.size]++;
    return 0;
}

SHOOT_RESULT board_fire_at(battle_board *brd, uint8_t row, uint8_t col) {
    if (row >= brd->size || col >= brd->size) {
        return NOT_ALLOWED;
    }
    char cell = brd->field[row][col];

    switch (cell) {
    case ' ':
        brd->mask[row][col] = '0';
        return MISSED_HIT;
    case '0':
    case 'X':
        return NOT_ALLOWED;
    case '#':
        fill_both_arr(brd, row, col, 'X');
        return board_check_ship_destroyed(brd, row, col);
    } 
    return NOT_ALLOWED;
}

void board_event_destroyed(battle_board *brd, uint8_t row, uint8_t col, uint8_t is_horizontal) {
    if (is_horizontal) {
        int8_t j = 0;
        // find left corner
        for (j = col; j >= 0 && brd->field[row][j] == 'X'; j--);
        // fill left side 
        if (j >= 0) {
            fill_both_arr(brd, row, j, '0');
            if (row > 0)                fill_both_arr(brd, row-1, j, '0');
            if (row < brd->size - 1)    fill_both_arr(brd, row+1, j, '0');
        }
        // fill ship neigh cells
        for (j++; j < brd->size && brd->field[row][j] == 'X'; j++) {
            if (row > 0)                fill_both_arr(brd, row-1, j, '0');
            if (row < brd->size - 1)    fill_both_arr(brd, row+1, j, '0');
        }
        // fill right side
        if (j < brd->size - 1) {
            fill_both_arr(brd, row, j, '0');
            if (row > 0)                fill_both_arr(brd, row-1, j, '0');
            if (row < brd->size - 1)    fill_both_arr(brd, row+1, j, '0');
        }
    }
    else {
        int8_t i = 0;
        // find up corner
        for (i = row; i >= 0 && brd->field[i][col] == 'X'; i--);
        // fill up side 
        if (i >= 0) {
            fill_both_arr(brd, i, col, '0');
            if (col > 0)                fill_both_arr(brd, i, col-1, '0');
            if (col < brd->size - 1)    fill_both_arr(brd, i, col+1, '0');
        }
        // fill ship neigh cells
        for (i++; i < brd->size && brd->field[i][col] == 'X'; i++) {
            if (col > 0)                fill_both_arr(brd, i, col-1, '0');
            if (col < brd->size - 1)    fill_both_arr(brd, i, col+1, '0');
        }
        // fill down side
        if (i < brd->size - 1) {
            fill_both_arr(brd, i, col, '0');
            if (col > 0)                fill_both_arr(brd, i, col-1, '0');
            if (col < brd->size - 1)    fill_both_arr(brd, i, col+1, '0');
        }
    }
}

int board_set_rand_ships(battle_board *brd, const uint8_t *ship_gen_counts) {
    uint8_t ship_counts[SHIP_MAX_SIZE];
    memcpy(ship_counts, ship_gen_counts, sizeof(*ship_gen_counts) * SHIP_MAX_SIZE);
    ship temp_ship;

    for(int i = SHIP_MAX_SIZE; i >= SHIP_MIN_SIZE && ship_counts[i - 1] > 0;) {
        temp_ship.x = rand() % brd->size;
        temp_ship.y = rand() % brd->size;
        temp_ship.size = i;
        temp_ship.is_horizontal = rand() % 2;
        if (board_set_ship(brd, temp_ship) == 0 && --ship_counts[i - 1] == 0) {  
            brd->ship_counts[SHIP_COUNT_POS] += ship_gen_counts[i - 1];
            //printf ("Set ship succesful: {%d %d %d %d}\n", temp_ship.x, temp_ship.y, temp_ship.is_horizontal, temp_ship.size);
            i--;
        }
    }
    return true;
}

void game_loop(battle_board *brd1, PLAYER_TYPE pt1, battle_board *brd2, PLAYER_TYPE pt2) {
    int8_t setup_flag = 1;
    while (setup_flag) {
        uint8_t counts[SHIP_MAX_SIZE] = {4, 3, 2, 1}; 
        // 1st player set ships  
        board_set_rand_ships(brd1, counts);
        printf("Player 1 table\n");
        board_print_both(brd1);
        // 2nd player set ships
        board_set_rand_ships(brd2, counts);
        printf("Player 2 table\n");
        board_print_both(brd2);
        setup_flag = 0;
    }
    uint8_t winner = 0;
    uint8_t row, col;
    while (!winner) {
        // player brd1
        while(true) {
            board_print_battlefield(brd1, brd2);
            printf("Player 1 turns\n");
            if (get_coor_user_input(stdin, brd2->size, &row, &col) < 0)
                continue;
            // turn 1st player
            SHOOT_RESULT status_shot = board_fire_at(brd2, row, col);
            if (status_shot == SUCCESS_FINISH_HIT) {
                if (brd2->ship_counts[SHIP_COUNT_POS] == 0) {
                    winner = 1;
                    break;
                }
                continue;
            }
            if (status_shot != MISSED_HIT) {
                // Shoot again
                continue;
            }
            break;
        }
        // turn 2nd player
        while(true) {
            board_print_battlefield(brd1, brd2);
            printf("Player 2 turns\n");
            if (get_coor_user_input(stdin, brd1->size, &row, &col) < 0)
                continue;
            // turn 1st player
            SHOOT_RESULT status_shot = board_fire_at(brd1, row, col);
            if (status_shot == SUCCESS_FINISH_HIT) {
                if (brd1->ship_counts[SHIP_COUNT_POS] == 0) {
                    winner = 2;
                    break;
                }
                continue;
            }
            if (status_shot != MISSED_HIT) {
                // Shoot again
                continue;
            }
            break;
        }
    }
}