#include "battle_cli.h"
#include "battle_ship.h"

#include <cctype>   
#include <iostream>
#include <sstream>
#include <algorithm>

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

void board_print_both(const BattleBoard &brd) { //, BOARD_TYPE ftype) {
    // print alph
    printf("  ");
    for (int i = 0; i < brd.get_rows(); i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\t\t\t");
    for (int i = 0; i < brd.get_rows(); i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\n");

    for (int i = 0; i < brd.get_rows(); i++) {
        printf("  ");
        for (int j = 0; j < brd.get_cols(); j++)
            printf("+===");
        printf("+\t\t\t");
        for (int j = 0; j < brd.get_cols(); j++)
            printf("+===");
        printf("+\n%2d", i + 1);

        for (int j = 0; j < brd.get_cols(); j++) {
            printf("|%s", get_fill_string(brd.field(i,j)));
        }
        printf("|\t\t\t");
        for (int j = 0; j < brd.get_cols(); j++) {
            printf("|%s", get_fill_string(brd.mask(i,j)));
        }
        printf("|\n");
    }
    printf("  ");
    for (int i = 0; i < brd.get_rows(); i++)
        printf("+===");
    printf("+\t\t\t");
    for (int i = 0; i < brd.get_rows(); i++)
        printf("+===");
    
    printf("+\n\n");
}

void board_print_battlefield(const BattleBoard &brd1, const BattleBoard &brd2) {
    // print alph
    printf("%40s%40s\n","Player 1", "Player 2");
    printf("  ");
    for (int i = 0; i < brd1.get_rows(); i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\t\t\t  ");
    for (int i = 0; i < brd2.get_rows(); i++)
        printf("| %c ", 'A' + (char)i);
    printf("|\n");

    for (int i = 0; i < brd1.get_rows(); i++) {
        printf("  ");
        for (int j = 0; j < brd1.get_cols(); j++)
            printf("+===");
        printf("+\t\t\t  ");
        for (int j = 0; j < brd2.get_cols(); j++)
            printf("+===");
        printf("+\n%2d", i + 1);

        for (int j = 0; j < brd1.get_cols(); j++) {
            printf("|%s", get_fill_string(brd1.mask(i,j)));
        }
        printf("|\t\t\t");
        printf("%2d", i + 1);
        for (int j = 0; j < brd2.get_cols(); j++) {
            printf("|%s", get_fill_string(brd2.mask(i,j)));
        }
        printf("|\n");
    }
    printf("  ");
    for (int i = 0; i < brd1.get_rows(); i++)
        printf("+===");
    printf("+\t\t\t  ");
    for (int i = 0; i < brd2.get_rows(); i++)
        printf("+===");
    
    printf("+\n\n");
}

int get_coor_user_input(std::istream &in, int max_row, int max_col, int *row, int *col) {
    char col_letter = -1;
    *row = -1;
    printf("Enter cell: ");
    std::string inp;
    std::getline(in, inp);
    std::remove_if (inp.begin(), inp.end(), isspace);
    
    std::stringstream ss (inp);
    if (inp.size() < 2)
        return -1;

    if (isalpha(inp[0]))
        ss >> col_letter >> *row;
    else if (isdigit(inp[0]))
        ss >> *row >> col_letter;
    else
        return -1;

    if (toupper(col_letter) - 'A' >= max_col || *row > max_row || *row == 0) {
        return -1;
    }
    
    *col = toupper(col_letter) - 'A';
    *row -= 1;
    return 0;
    
}