#include "battle_ship.h"
#include "battle_cli.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define BOARD_SIZE 10

int main() {
    printf("---START DRIVER---\n");
    srand(time(NULL));

    battle_board brd_1, brd_2;

    if(board_init(&brd_1, BOARD_SIZE, ' ') != EXIT_SUCCESS) {
        printf("Error alloc board 1\n");
        return -1;
    }
    if(board_init(&brd_2, BOARD_SIZE, ' ') != EXIT_SUCCESS) {
        printf("Error alloc board 2\n");
        return -1;
    }

    game_loop(&brd_1, HUMAN_PLAYER_TYPE, &brd_2, HUMAN_PLAYER_TYPE);

    board_deinit(&brd_2);
    board_deinit(&brd_1);

    printf("---END DRIVER---\n");
    return 0;
}