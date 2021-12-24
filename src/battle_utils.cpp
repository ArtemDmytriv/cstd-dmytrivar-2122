#include "battle_utils.h"
#include "battle_ship.h"

#include <algorithm>
#include <Arduino.h>

GAME_STATE send_state(GAME_STATE new_gs) {
    char state_buffer[32] = {};
    String confirmStr;
    snprintf(state_buffer, 32, "%d", static_cast<int>(new_gs));

    if (new_gs != GAME_STATE::wait_sync) {
        while(confirmStr != state_buffer) {
            Serial.println(state_buffer);
            confirmStr = Serial.readString();
        }
    }
    else {
        Serial.println(state_buffer);
    }
    return new_gs;
}

void send_both_boards(BattleBoard *brd1, BattleBoard *brd2, bool send_mask) {
    std::string board_buffer;
    board_buffer.resize(brd1->get_size() + 2);
    board_buffer[0] = 'b';
    board_buffer[board_buffer.size() - 1] = 'e';

    if (send_mask)
        std::copy(&brd1->mask(0,0), &brd1->mask(0,0) + brd1->get_size(), board_buffer.begin() + 1);
    else 
        std::copy(&brd1->field(0,0), &brd1->field(0,0) + brd1->get_size(), board_buffer.begin() + 1);

    Serial.println(board_buffer.c_str());
    Serial.println(END_SEQ_MSG);   
    Serial.println("");

    if (send_mask)
        std::copy(&brd2->mask(0,0), &brd2->mask(0,0) + brd2->get_size(), board_buffer.begin() + 1);
    else 
        std::copy(&brd2->field(0,0), &brd2->field(0,0) + brd2->get_size(), board_buffer.begin() + 1);
    Serial.println(board_buffer.c_str());
    Serial.println(END_SEQ_MSG);   
}

int get_shot_coor(int &x, int &y) {
    String shootStr;
    shootStr = Serial.readString();
    if (shootStr.indexOf("s>") != 0) {
        return 1;
    }
    if(sscanf(shootStr.c_str(), "s>%d %d", &x, &y) != 2) {
        return 2;
    }
    return 0;
}

int send_winner(int winner) {
    char buffer[8];
    snprintf(buffer, 8, "w%d", winner);
    Serial.println(buffer);
    Serial.println(buffer);
}

GAME_STATE load_game_from_serial(BattleBoard *brd1, PLAYER_TYPE &p1, BattleBoard *brd2, PLAYER_TYPE &p2) {
    String str;
    p1 = PLAYER_TYPE(Serial.readString().toInt());
    str = Serial.readString();
    std::copy(str.c_str(), str.c_str() + str.length(), &brd1->field(0, 0));
    str = Serial.readString();
    std::copy(str.c_str(), str.c_str() + str.length(), &brd1->mask(0, 0));

    p2 = PLAYER_TYPE(Serial.readString().toInt());
    str = Serial.readString();
    std::copy(str.c_str(), str.c_str() + str.length(), &brd2->field(0, 0));
    str = Serial.readString();
    std::copy(str.c_str(), str.c_str() + str.length(), &brd2->mask(0, 0));

    return GAME_STATE(Serial.readString().toInt());
}