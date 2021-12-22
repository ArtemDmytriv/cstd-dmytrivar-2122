#include "battle_ship.h"

#ifdef CLI_COMPILATION
#include "battle_cli.h"
#else 
#include "Arduino.h"
#include "SPI.h"
#include "battle_utils.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

#define BOARD_SIZE 10

#ifndef CLI_COMPILATION
GAME_STATE current_state = GAME_STATE::wait_sync;
char resp_buffer[SERIAL_BUFFER_LEN] = {};

// battle boards
BattleBoard *brd_p1, *brd_p2;
std::string board_buffer;
std::vector<std::pair<int, int>> ship_conf =  {{4, 1}, {3, 2}, {2, 3}, {1, 4}};

void setup() {
    randomSeed(analogRead(0));
    brd_p1 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);
    brd_p2 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);

    board_buffer.resize(BOARD_SIZE * BOARD_SIZE + 2);
    board_buffer.front() = 'b';
    board_buffer.back() = 'e';
    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, LOW);

    current_state = GAME_STATE::wait_sync;
    Serial.begin(9600);
    Serial.setTimeout(100);
}


char incomingByte;
String str;
int x, y = 0;
int mode = 0;
int winner = -1;

void loop() {
    switch (current_state) {
        // -> sync request
        case GAME_STATE::wait_sync : {
            Serial.println(SYNC_MSG);
            if (Serial.available() > 0) {
                incomingByte = Serial.read();
                if (incomingByte == '#')
                    current_state = send_state(GAME_STATE::main_menu);
            }
            delay(500);
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
                        current_state = send_state(GAME_STATE::setup_p1_board);
                    }
                }
            }
            break;
        }
        case GAME_STATE::setup_p1_board : {
            brd_p1->board_set_rand_ships(ship_conf, random);
            std::copy(&brd_p1->field(0,0), &brd_p1->field(0,0) + brd_p1->get_size(), board_buffer.begin() + 1);
            Serial.println(board_buffer.c_str());
            Serial.println(END_SEQ_MSG);
            
            current_state = send_state(GAME_STATE::setup_p2_board);
            break;
        }
        case GAME_STATE::setup_p2_board : {
            brd_p2->board_set_rand_ships(ship_conf, random);
            std::copy(&brd_p2->field(0,0), &brd_p2->field(0,0) + brd_p2->get_size(), board_buffer.begin() + 1);
            Serial.println(board_buffer.c_str());
            Serial.println(END_SEQ_MSG);

            current_state = send_state(GAME_STATE::turns_p1);
            break;
        }
        case GAME_STATE::turns_p1 : {
            send_both_boards(brd_p1, brd_p2, true);

            while(get_shot_coor(x, y) != EXIT_SUCCESS);

            auto sh_status = brd_p2->board_fire_at(x, y);
            if (sh_status == SHOOT_RESULT::MISSED_HIT) {
                current_state = send_state(GAME_STATE::turns_p2);
            }
            else if (sh_status == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                if (brd_p2->get_total_alive_count() == 0) {
                    winner = 1;
                    current_state = send_state(GAME_STATE::announce_winner);
                }
                else { 
                    current_state = send_state(GAME_STATE::turns_p1);
                }
            }
            else {
                current_state = send_state(GAME_STATE::turns_p1);
            }
            break;
        }
        case GAME_STATE::turns_p2 : {
            send_both_boards(brd_p1, brd_p2, true);

            while(get_shot_coor(x, y) != EXIT_SUCCESS);

            auto sh_status = brd_p1->board_fire_at(x, y);
            if (sh_status == SHOOT_RESULT::MISSED_HIT) {
                current_state = send_state(GAME_STATE::turns_p1);
            }
            else if (sh_status == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                if (brd_p1->get_total_alive_count() == 0) {
                    winner = 2;
                    current_state = send_state(GAME_STATE::announce_winner);
                }
                else {
                    current_state = send_state(GAME_STATE::turns_p2);
                }
            }
            else {
                current_state = send_state(GAME_STATE::turns_p2);
            }
            break;
        }
        case GAME_STATE::announce_winner : {
            send_both_boards(brd_p1, brd_p2, false);
            send_winner(winner);
            delay(10000);
            current_state = send_state(GAME_STATE::wait_sync);
            break;
        }
    }
}
#else
int main() {
    printf("---START DRIVER---\n");
    srand(time(NULL));

    BattleBoard brd_1(BOARD_SIZE, BOARD_SIZE), 
        brd_2(BOARD_SIZE, BOARD_SIZE);

    game_loop(brd_1, PLAYER_TYPE::HUMAN_PLAYER_TYPE, brd_2, PLAYER_TYPE::HUMAN_PLAYER_TYPE);

    printf("---END DRIVER---\n");
    return 0;
}
#endif