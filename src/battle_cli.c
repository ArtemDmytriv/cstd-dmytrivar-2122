#include "battle_cli.h"
#include "battle_ship.h"
#include <ctype.h>   

const int BUFLEN = 64;

const char* get_fill_string(char ch) {
    switch (ch) {
        case 'X' : return SHOOT_SHIP_CELL;
        case '#' : return SHIP_CELL;
        case ' ' : return EMPTY_CELL;
        case '0' : return MISSED_CELL;
    }
    return EMPTY_CELL;
}

void board_print_both(battle_board *brd) { //, BOARD_TYPE ftype) {
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
        printf("+\n%2lu", i + 1);

        for (size_t j = 0; j < brd->size; j++) {
            printf("|%s", get_fill_string(brd->field[i][j]));
        }
        printf("|\t\t\t");
        for (size_t j = 0; j < brd->size; j++) {
            printf("|%s", get_fill_string(brd->mask[i][j]));
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

void board_print_battlefield(battle_board *brd1, battle_board *brd2) {
    // print alph
    printf("%40s%40s\n","Player 1", "Player 2");
    printf("  ");
    for (size_t i = 0; i < brd1->size; i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\t\t\t  ");
    for (size_t i = 0; i < brd2->size; i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\n");

    for (size_t i = 0; i < brd1->size; i++) {
        printf("  ");
        for (size_t i = 0; i < brd1->size; i++)
            printf("+===");
        printf("+\t\t\t  ");
        for (size_t i = 0; i < brd2->size; i++)
            printf("+===");
        printf("+\n%2lu", i + 1);

        for (size_t j = 0; j < brd1->size; j++) {
            printf("|%s", get_fill_string(brd1->mask[i][j]));
        }
        printf("|\t\t\t");
        printf("%2lu", i + 1);
        for (size_t j = 0; j < brd2->size; j++) {
            printf("|%s", get_fill_string(brd2->mask[i][j]));
        }
        printf("|\n");
    }
    printf("  ");
    for (size_t i = 0; i < brd1->size; i++)
        printf("+===");
    printf("+\t\t\t  ");
    for (size_t i = 0; i < brd2->size; i++)
        printf("+===");
    
    printf("+\n\n");
}

int get_coor_user_input(FILE *in, uint8_t size, uint8_t *row, uint8_t *col) {
    char buffer[BUFLEN] = {};
    char col_letter = -1;
    printf("Enter cell: ");
    fgets(buffer, BUFLEN, in);
    if (sscanf(buffer, "%hhu%c", row, &col_letter) != 2) {
        if (sscanf(buffer, "%c%hhu", &col_letter, row) != 2) {
            return -1;
        }
    }
    if (!isalpha(col_letter) || toupper(col_letter) - 'A' >= size || *row > size) {
        return -1;
    }
    *col = toupper(col_letter) - 'A';
    *row -= 1;
    return 0;
}