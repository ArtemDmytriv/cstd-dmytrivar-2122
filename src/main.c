#include "battle_ship.h"
#include "battle_cli.h"

#include <stdlib.h>
#include <stdio.h>

int main() {
    printf("---START DRIVER---\n");

    battle_board brd_1; // , brd_2;

    if(board_init(&brd_1, 10, ' ') != EXIT_SUCCESS) {
        printf("Error\n");
        return -1;
    }

    ship s1 = {2, 2, 4, false};
    board_set_ship(&brd_1, s1);

    board_set_rand_ships(&brd_1);
    board_print(&brd_1);

    board_deinit(&brd_1);

    printf("---END DRIVER---\n");
    return 0;
}