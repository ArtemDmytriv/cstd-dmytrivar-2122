#include "Arduino.h"

#include "battle_utils.h"
#include "battle_ship.h"

#ifdef CLI_COMPILATION
#include "battle_cli.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>

#define SYNC_MSG "#$SYNC$#"
#define SERIAL_BUFFER_LEN 128
#define BOARD_SIZE 10

GAME_STATE current_state = GAME_STATE::wait_sync;
char resp_buffer[SERIAL_BUFFER_LEN] = {};

void setup() {
    current_state = GAME_STATE::wait_sync;
    Serial.begin(9600);
    Serial.setTimeout(1000);
}

void loop() {
    char incomingByte;
    switch (current_state) {
        case GAME_STATE::wait_sync : {
            Serial.println(SYNC_MSG);
            if (Serial.available()) {
                incomingByte = Serial.read();
            }
            break;
        }
        case GAME_STATE::wait_response :
        case GAME_STATE::setup_p1_board :
        case GAME_STATE::setup_p2_board :
        case GAME_STATE::turns_p1 :
        case GAME_STATE::turns_p2 :
            break;
    }
    // -> sync request
    // <- sync approve

    // -> show menu (send all possible game modes)
    // <- get mode number from serial
    
    // -> set state to setup_p1, show request for random generate or full filled board
    // <- set board 1

    // -> set state to setup_p2, show request for random generate or full filled board
    // <- set board 2
    Serial.println("Hello world");
    delay(1000);
}

// int main() {
//     printf("---START DRIVER---\n");
//     srand(time(NULL));

//     BattleBoard brd_1(BOARD_SIZE, BOARD_SIZE), 
//         brd_2(BOARD_SIZE, BOARD_SIZE);

//     game_loop(brd_1, PLAYER_TYPE::HUMAN_PLAYER_TYPE, brd_2, PLAYER_TYPE::HUMAN_PLAYER_TYPE);

//     printf("---END DRIVER---\n");
//     return 0;
// }