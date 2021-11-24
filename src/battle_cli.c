#include "battle_cli.h"
#include "battle_ship.h"
#include <stdio.h>

void board_print(battle_board *brd) { //, BOARD_TYPE ftype) {
    // print alph
    printf("  ");
    for (size_t i = 0; i < brd->size; i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\t\t\t");
    for (size_t i = 0; i < brd->size; i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\n");

    for (size_t i = 0; i < brd->size; i++) {
        printf("  ");
        for (size_t i = 0; i < brd->size; i++)
            printf("+===");
        printf("+\t\t\t");
        for (size_t i = 0; i < brd->size; i++)
            printf("+===");
        printf("+\n%2llu", i + 1);

        for (size_t j = 0; j < brd->size; j++) {
            char ch = brd->field[i][j];
            switch (ch) {
            case 'X' : printf("|%s", SHOOT_SHIP_CELL); break;
            case '#' : printf("|%s", SHIP_CELL); break;
            case ' ' : printf("|%s", EMPTY_CELL); break;
            case '0' : printf("|%s", MISSED_CELL); break;
            }
        }
        printf("|\t\t\t");
        for (size_t j = 0; j < brd->size; j++) {
            char ch = brd->mask[i][j];
            switch (ch) {
            case 'X' : printf("|%s", SHOOT_SHIP_CELL); break;
            case '#' : printf("|%s", SHIP_CELL); break;
            case ' ' : printf("|%s", EMPTY_CELL); break;
            case '0' : printf("|%s", MISSED_CELL); break;
            }
        }
        printf("|\n");
    }
    printf("  ");
    for (size_t i = 0; i < brd->size; i++)
        printf("+===");
    printf("+\t\t\t");
    for (size_t i = 0; i < brd->size; i++)
        printf("+===");
    
    printf("+\n\n");
}