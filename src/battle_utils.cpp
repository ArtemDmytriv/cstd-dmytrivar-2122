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

String get_substring(const String &str, const char *beg, const char *end) {
    int index_beg = -1,
        index_end = -1;
    if ((index_beg = str.indexOf(beg)) < 0) {
        return "";
    }
    if ((index_end = str.indexOf(end, index_beg)) < 0 ) {
        return "";
    }
    return str.substring(index_beg + strlen(beg), index_end);
}

GAME_STATE load_save(BattleBoard *brd1, PLAYER_TYPE &p1, BattleBoard *brd2, PLAYER_TYPE &p2) {
    // String str = "";
    // while(str.substring(0,2) != "p1") str = Serial.readString();
    // p1 = PLAYER_TYPE(get_substring(str, "p1<", ">").toInt());
    
    // while(str.substring(0,2) != "p2") str = Serial.readString();
    // p2 = PLAYER_TYPE(get_substring(str, "p2<", ">").toInt());

    // while(str.substring(0,2) != "f1") str = Serial.readString();
    // std::copy(str.c_str() + 3, str.c_str() + 3 + brd1->get_size(), &brd1->field(0, 0));

    // while(str.substring(0,2) != "m1") str = Serial.readString();
    // std::copy(str.c_str() + 3, str.c_str() + 3 + brd1->get_size(), &brd1->mask(0, 0));

    // while(str.substring(0,2) != "f2") str = Serial.readString();
    // std::copy(str.c_str() + 3, str.c_str() + 3 + brd2->get_size(), &brd2->field(0, 0));

    // while(str.substring(0,2) != "m2") str = Serial.readString();
    // std::copy(str.c_str() + 3, str.c_str() + 3 + brd2->get_size(), &brd2->mask(0, 0));

    // while(str.substring(0,1) != "s") str = Serial.readString();
    // return GAME_STATE(get_substring(str, "s<", ">").toInt());
}

// GAME_STATE parse_save_from_serial(const String &str, BattleBoard *brd1, PLAYER_TYPE &p1, BattleBoard *brd2, PLAYER_TYPE &p2) {
//     String temp = get_substring(str, "p1<", ">");
//     Serial.println(temp);
//     p1 = PLAYER_TYPE(temp.toInt()); 

//     temp = get_substring(str, "p2<", ">");
//     Serial.println(temp);
//     p2 = PLAYER_TYPE(temp.toInt()); 
    
//     temp = get_substring(str, "f1<", ">");
//     Serial.println(temp);
//     std::copy(temp.c_str(), temp.c_str() + temp.length(), &brd1->field(0,0));

//     //temp = get_substring(str, "m1<", ">");
//     //Serial.println(temp);
//     //std::copy(temp.c_str(), temp.c_str() + temp.length(), &brd1->mask(0,0));

//     //temp = get_substring(str, "f2<", ">");
//     //Serial.println(temp);
//     //std::copy(temp.c_str(), temp.c_str() + temp.length(), &brd2->field(0,0));

//     //temp = get_substring(str, "m2<", ">");
//     //Serial.println(temp);
//     //std::copy(temp.c_str(), temp.c_str() + temp.length(), &brd2->mask(0,0));

//     temp = get_substring(str, "s<", ">");    
//     Serial.println(temp);
//     return GAME_STATE(temp.toInt());
// }