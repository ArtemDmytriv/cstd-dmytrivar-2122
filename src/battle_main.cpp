#include "battle_ship.h"
#include "battle_ship_ai.h"

#ifdef CLI_COMPILATION
#include "battle_cli.h"
#include <ctime>
#include <vector>
#else
#include "Arduino.h"
//#include "SPI.h"
#include "battle_utils.h"
#endif

#include <cstdlib>

#define BOARD_SIZE 10

#ifndef CLI_COMPILATION
GAME_STATE current_state = GAME_STATE::wait_sync;

// battle boards
BattleBoard *brd_p1;
BattleBoard *brd_p2;

AI_BattleShip *ai1;
unsigned char ai1_turns_buffer[BOARD_SIZE * BOARD_SIZE];

AI_BattleShip *ai2;
unsigned char ai2_turns_buffer[BOARD_SIZE * BOARD_SIZE];

char board_buffer[BOARD_SIZE * BOARD_SIZE + 2];
std::vector<std::pair<int, int>> ship_conf =  {{4, 1}, {3, 2}, {2, 3}, {1, 4}};

void setup() {
    randomSeed(analogRead(0));
    brd_p1 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);
    brd_p2 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);

    ai1 = new AI_BattleShip(ai1_turns_buffer, random);
    ai2 = new AI_BattleShip(ai2_turns_buffer, random);

    board_buffer[0] = 'b';
    board_buffer[BOARD_SIZE * BOARD_SIZE + 1] = 'e';

    current_state = GAME_STATE::wait_sync;
    Serial.begin(115200);
    Serial.setTimeout(200);
}



String str;
int x, y = 0;
GAME_MODES mode = GAME_MODES::player_vs_player;
GAME_MENU menu = GAME_MENU::new_game;

SHOOT_RESULT sh_status;
PLAYER_TYPE p1 = PLAYER_TYPE::HUMAN_PLAYER_TYPE,
            p2 = PLAYER_TYPE::HUMAN_PLAYER_TYPE;

int winner = -1;

void loop() {
    switch (current_state) {
        case GAME_STATE::wait_sync : {
            Serial.println(SYNC_MSG);
            if (Serial.available() > 0) {
                char incomingByte = Serial.read();
                if (incomingByte == '#')
                    current_state = send_state(GAME_STATE::main_menu);
            }
            delay(500);
            break;
        }
        case GAME_STATE::main_menu : {
            for (const auto &mode : vec_menu) {
                Serial.println(mode.c_str());
            }
            Serial.println(END_SEQ_MSG);
            current_state = GAME_STATE::wait_response;
            break;
        }
        case GAME_STATE::mode_menu : {
            for (const auto &mode : vec_modes) {
                Serial.println(mode.c_str());
            }
            Serial.println(END_SEQ_MSG);
            current_state = GAME_STATE::wait_response;
            break;
        }
        case GAME_STATE::load_game : {
            current_state = load_game_from_serial(brd_p1, p1, brd_p2, p2);
            break;
        }
        case GAME_STATE::wait_response : {
            if (Serial.available() > 0) {
                str = Serial.readString();
                if (str[0] == 't' && str.length() > 1 && str.substring(1).toInt() != 0) {
                    Serial.println(str);
                    menu = GAME_MENU(str.substring(1).toInt() - 1);
                    switch (menu) {
                        case GAME_MENU::new_game : {      
                            current_state = send_state(GAME_STATE::mode_menu);
                            break;
                        }
                        case GAME_MENU::load_game : {
                            current_state = send_state(GAME_STATE::load_game);
                            break;
                        }
                    }
                    Serial.println();
                }
                else if (str[0] == 'm' && str.length() > 1 && str.substring(1).toInt() != 0) {
                    Serial.println(str);
                    mode = GAME_MODES(str.substring(1).toInt() - 1);
                    switch (mode) {
                    case GAME_MODES::player_vs_player :
                        p1 = PLAYER_TYPE::HUMAN_PLAYER_TYPE;
                        p2 = PLAYER_TYPE::HUMAN_PLAYER_TYPE;
                        break;
                    case GAME_MODES::player_vs_ai :
                        p1 = PLAYER_TYPE::HUMAN_PLAYER_TYPE;
                        p2 = PLAYER_TYPE::AI_PLAYER_TYPE;
                        break;
                    case GAME_MODES::ai_vs_ai :
                        p1 = PLAYER_TYPE::AI_PLAYER_TYPE;
                        p2 = PLAYER_TYPE::AI_PLAYER_TYPE;
                        break;
                    }
                    Serial.println();
                    current_state = send_state(GAME_STATE::setup_p1_board);
                }
            }
            break;
        }
        case GAME_STATE::setup_p1_board : {
            brd_p1->board_set_rand_ships(ship_conf, random);
            std::copy(&brd_p1->field(0,0), &brd_p1->field(0,0) + brd_p1->get_size(), board_buffer + 1);
            Serial.println(board_buffer); 
            Serial.println(END_SEQ_MSG);
            if (p1 == PLAYER_TYPE::AI_PLAYER_TYPE) {
                ai1->init_moves(BOARD_SIZE, BOARD_SIZE);
            }
            Serial.println();
            current_state = send_state(GAME_STATE::setup_p2_board);
            break;
        }
        case GAME_STATE::setup_p2_board : {
            brd_p2->board_set_rand_ships(ship_conf, random);
            std::copy(&brd_p2->field(0,0), &brd_p2->field(0,0) + brd_p2->get_size(), board_buffer + 1);
            Serial.println(board_buffer);
            Serial.println(END_SEQ_MSG);
            if (p2 == PLAYER_TYPE::AI_PLAYER_TYPE) {
                ai2->init_moves(BOARD_SIZE, BOARD_SIZE);
            }
            Serial.println();
            current_state = send_state(p1 == PLAYER_TYPE::HUMAN_PLAYER_TYPE? GAME_STATE::turns_p1 : GAME_STATE::turns_AI1);
            break;
        }
        case GAME_STATE::turns_p1 : {
            send_both_boards(brd_p1, brd_p2, true);

            while(get_shot_coor(x, y) != EXIT_SUCCESS);

            sh_status = brd_p2->board_fire_at(x, y);
            if (sh_status == SHOOT_RESULT::MISSED_HIT) {
                current_state = send_state(p2 == PLAYER_TYPE::HUMAN_PLAYER_TYPE? GAME_STATE::turns_p2 : GAME_STATE::turns_AI2);
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

            sh_status = brd_p1->board_fire_at(x, y);
            if (sh_status == SHOOT_RESULT::MISSED_HIT) {
                current_state = send_state(p1 == PLAYER_TYPE::HUMAN_PLAYER_TYPE? GAME_STATE::turns_p1 : GAME_STATE::turns_AI1);
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
        case GAME_STATE::turns_AI1 : {
            send_both_boards(brd_p1, brd_p2, true);

            sh_status = ai1->fire(brd_p2);
            if (sh_status == SHOOT_RESULT::MISSED_HIT) {
                current_state = send_state(p2 == PLAYER_TYPE::HUMAN_PLAYER_TYPE? GAME_STATE::turns_p2 : GAME_STATE::turns_AI2);
            }
            else if (sh_status == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                if (brd_p2->get_total_alive_count() == 0) {
                    winner = 1;
                    current_state = send_state(GAME_STATE::announce_winner);
                }
                else {
                    current_state = send_state(GAME_STATE::turns_AI1);
                }
            }
            else {
                current_state = send_state(GAME_STATE::turns_AI1);
            }
            break;
        }
        case GAME_STATE::turns_AI2 : {
            send_both_boards(brd_p1, brd_p2, true);

            sh_status = ai2->fire(brd_p1);
            if (sh_status == SHOOT_RESULT::MISSED_HIT) {
                current_state = send_state(p1 == PLAYER_TYPE::HUMAN_PLAYER_TYPE? GAME_STATE::turns_p1 : GAME_STATE::turns_AI1);
            }
            else if (sh_status == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                if (brd_p1->get_total_alive_count() == 0) {
                    winner = 2;
                    current_state = send_state(GAME_STATE::announce_winner);
                }
                else {
                    current_state = send_state(GAME_STATE::turns_AI2);
                }
            }
            else {
                //Serial.println("SUCCESS or NOT_ALLOWED");
                current_state = send_state(GAME_STATE::turns_AI2);
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

long wrap_rand() {
    return rand();
}

void game_loop(BattleBoard &brd1, PLAYER_TYPE pt1, BattleBoard &brd2, PLAYER_TYPE pt2) {
    bool setup_flag = true;
    AI_BattleShip ai1(wrap_rand),
                ai2(wrap_rand);
    while (setup_flag) {
        std::vector<std::pair<int,int>> counts = {{1, 4}, {2, 3}, {3, 2}, {4, 1}}; 
        // 1st player set ships  
        brd1.board_set_rand_ships(counts, wrap_rand);
        printf("Player 1 table\n");
        board_print_both(brd1);
        // 2nd player set ships
        brd2.board_set_rand_ships(counts, wrap_rand);
        printf("Player 2 table\n");
        board_print_both(brd2);
        
        if (pt1 == PLAYER_TYPE::AI_PLAYER_TYPE) {
            ai1.init_moves(brd2.get_rows(), brd2.get_cols());
        }
        if (pt2 == PLAYER_TYPE::AI_PLAYER_TYPE) {
            ai2.init_moves(brd1.get_rows(), brd1.get_cols());
        }
        setup_flag = false;
    }
    int winner = 0;
    int row, col;
    SHOOT_RESULT last_ai1_shoot, last_ai2_shoot;
    while (!winner) {
        // player brd1
        while(true) {
            board_print_battlefield(brd1, brd2);
            if (pt1 == PLAYER_TYPE::HUMAN_PLAYER_TYPE) {
                printf("Player 1 turns\n");
                if (get_coor_user_input(std::cin, brd2.get_rows(), brd2.get_cols(), &row, &col) < 0)
                    continue;
                // turn 1st player
                SHOOT_RESULT status_shot = brd2.board_fire_at(row, col);
                if (status_shot == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                    if (brd2.get_total_alive_count() == 0) {
                        winner = 1;
                        break;
                    }
                    continue;
                }
                if (status_shot != SHOOT_RESULT::MISSED_HIT) {
                    // Shoot again
                    continue;
                }
                break;
            }
            else { // AI
                last_ai1_shoot = ai1.fire(&brd2);
                if (last_ai1_shoot == SHOOT_RESULT::MISSED_HIT) {
                    break;
                }
                else if (last_ai1_shoot == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                    ai1.reset_vars();
                    if (brd2.get_total_alive_count() == 0) {
                        winner = 1;
                        break;
                    }
                }
            }
        }
        // turn 2nd player
        while(true) {
            board_print_battlefield(brd1, brd2);
            if (pt2 == PLAYER_TYPE::HUMAN_PLAYER_TYPE) {
                printf("Player 2 turns\n");
                if (get_coor_user_input(std::cin, brd1.get_rows(), brd1.get_cols(), &row, &col) < 0)
                    continue;
                // turn 1st player
                SHOOT_RESULT status_shot = brd1.board_fire_at(row, col);
                if (status_shot == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                    if (brd1.get_total_alive_count() == 0) {
                        winner = 2;
                        break;
                    }
                    continue;
                }
                if (status_shot != SHOOT_RESULT::MISSED_HIT) {
                    // Shoot again
                    continue;
                }
                break;
            }
            else { // AI
                last_ai2_shoot = ai2.fire(&brd1);
                if (last_ai2_shoot == SHOOT_RESULT::MISSED_HIT) {
                    break;
                }
                else if (last_ai2_shoot == SHOOT_RESULT::SUCCESS_FINISH_HIT) {
                    ai2.reset_vars();
                    if (brd1.get_total_alive_count() == 0) {
                        winner = 1;
                        break;
                    }
                }
            }
        }
    }
}

int main() {
    printf("---START DRIVER---\n");
    srand(time(NULL));

    BattleBoard brd_1(BOARD_SIZE, BOARD_SIZE), 
        brd_2(BOARD_SIZE, BOARD_SIZE);

    game_loop(brd_1, PLAYER_TYPE::AI_PLAYER_TYPE, brd_2, PLAYER_TYPE::AI_PLAYER_TYPE);

    printf("---END DRIVER---\n");
    return 0;
}
#endif