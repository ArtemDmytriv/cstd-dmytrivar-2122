#include "battle_ship.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int board_init(battle_board *brd, size_t size, char fill) {
    brd->size = size;
    brd->ship_count = 0;
    size_t new_2d_size = size * sizeof(char**) + size * size; 

    brd->field = (char**)malloc(new_2d_size);
    if (!brd->field)
        return EXIT_FAILURE;
    for(size_t i = 0; i < size; i++) {
        brd->field[i] = (char*)(brd->field + size) + size * i;
    }
    memset(brd->field[0], fill, size * size);

    brd->mask = (char**)malloc(new_2d_size);
    if (!brd->mask)
        return EXIT_FAILURE;
    for(size_t i = 0; i < size; i++)
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

SHOOT_RESULT board_check_ship_destroyed(battle_board *brd, size_t row, size_t col) {
    
    return SUCCESS_HIT;
}

int board_set_ship(battle_board *brd, ship sh) {
    for (size_t i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        if (sh.x >= brd->size || sh.x >= brd->size || 
            brd->field[sh.x][sh.y] != ' ')
        {
            return false;
        }
    }
    if (sh.is_horizontal) sh.y -= sh.size;
    else sh.x -= sh.size;

    for (size_t i = 0; i < sh.size; (sh.is_horizontal)? ++sh.y : ++sh.x, i++) {
        brd->field[sh.x][sh.y] = '#';
    }

    return true;
}

SHOOT_RESULT board_fire_at(battle_board *brd, size_t row, size_t col) {
    char cell = brd->field[row][col];
    
    switch (cell) {
    case ' ':
        brd->mask[row][col] = '0';
        return MISSED_HIT;
    case '0':
    case 'X':
        return NOT_ALLOWED;
    case '#':
        brd->mask[row][col] = 'X';
        brd->field[row][col] = 'X';
        return board_check_ship_destroyed(brd, row, col);
    } 
    return NOT_ALLOWED;
}

int board_set_rand_ships(battle_board *brd) {
    return 1;
}