#include "Arduino.h"

#include "battle_utils.h"
#include "battle_ship.h"

#ifdef CLI_COMPILATION
#include "battle_cli.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

#define BOARD_SIZE 10

GAME_STATE current_state = GAME_STATE::wait_sync;
char resp_buffer[SERIAL_BUFFER_LEN] = {};

// battle boards
BattleBoard *brd_p1, *brd_p2;
std::string board_buffer;
std::vector<std::pair<int, int>> ship_conf =  {{4, 1}, {3, 2}, {2, 3}, {1, 4}};

void setup() {
    srand(time(NULL));
    brd_p1 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);
    brd_p2 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);

    board_buffer.resize(BOARD_SIZE * BOARD_SIZE + 2);
    board_buffer.front() = 'b';
    board_buffer.back() = 'e';
    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, LOW);

    current_state = GAME_STATE::wait_sync;
    Serial.begin(9600);
    Serial.setTimeout(1000);
}


char incomingByte;
String str;
int mode = 0;

void loop() {
    switch (current_state) {
        // -> sync request
        case GAME_STATE::wait_sync : {
            Serial.println(SYNC_MSG);
            if (Serial.available() > 0) {
                incomingByte = Serial.read();
                if (incomingByte == '#')
                    current_state = send_state(current_state, GAME_STATE::main_menu);
            }
            break;
        }
        // <- sync approve
        case GAME_STATE::main_menu : {
            for (const auto &mode : vec_modes) {
                Serial.println(mode.c_str());
            }
            Serial.println(END_SEQ_MSG);
            current_state = GAME_STATE::wait_response;
            break;
        }
        case GAME_STATE::wait_response : {
            if (Serial.available() > 0) {
                str = Serial.readString();
                if (str[0] = 'm' && str.length() > 1) {
                    // remove m char
                    str = str.substring(1);
                    mode = str.toInt();
                    if (mode == 1) {
                        current_state = send_state(current_state, GAME_STATE::setup_p1_board);
                    }
                }
            }
            break;
        }
        case GAME_STATE::setup_p1_board : {
            brd_p1->board_set_rand_ships(ship_conf);
            std::copy(&brd_p1->field(0,0), &brd_p1->field(0,0) + BOARD_SIZE * BOARD_SIZE, board_buffer.begin() + 1);
            
            Serial.println(board_buffer.c_str());
            Serial.println(END_SEQ_MSG);
            
            current_state = send_state(current_state, GAME_STATE::setup_p2_board);
            break;
        }
        case GAME_STATE::setup_p2_board : {
            brd_p2->board_set_rand_ships(ship_conf);
            std::copy(&brd_p2->field(0,0), &brd_p2->field(0,0) + BOARD_SIZE * BOARD_SIZE, board_buffer.begin() + 1);

            Serial.println(board_buffer.c_str());
            Serial.println(END_SEQ_MSG);

            current_state = send_state(current_state, GAME_STATE::turns_p1);
            break;
        }
        case GAME_STATE::turns_p1 : {
            std::copy(&brd_p2->mask(0,0), &brd_p2->mask(0,0) + BOARD_SIZE * BOARD_SIZE, board_buffer.begin() + 1);
            Serial.println(board_buffer.c_str());
            Serial.println(END_SEQ_MSG);

            
            delay(4000);
            break;
        }
    }
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