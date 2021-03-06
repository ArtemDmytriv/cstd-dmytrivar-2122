#include "battle_utils.h"

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
    char board_buffer[BOARD_SIZE * BOARD_SIZE + 3] = {};
    board_buffer[0] = 'b';
    board_buffer[BOARD_SIZE * BOARD_SIZE + 1] = 'e';

    if (send_mask)
        memcpy(board_buffer + 1, &brd1->mask(0,0), brd1->get_size());
    else
        memcpy(board_buffer + 1, &brd1->field(0,0), brd1->get_size());

    Serial.println(board_buffer);
    Serial.println(END_SEQ_MSG);
    Serial.println("");

    if (send_mask)
        memcpy(board_buffer + 1, &brd2->mask(0,0), brd2->get_size());
    else
        memcpy(board_buffer + 1, &brd2->field(0,0), brd2->get_size());
    Serial.println(board_buffer);
    Serial.println(END_SEQ_MSG);
    Serial.println("");
}

int get_shot_coor(int &x, int &y) {
    String shootStr;
    shootStr = Serial.readString();
    if (shootStr == "save") {
        return -1;
    }
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

void send_ships_count(BattleBoard *brd) {
    String comma = ",";
    String str = "";
    str = brd->get_alive_count(1) + comma + brd->get_alive_count(2) + comma + brd->get_alive_count(3) + comma + brd->get_alive_count(4); 
    Serial.println(str);
}

void get_ships_count_from_save(const String &str, BattleBoard *brd1) {
    char buffer[32];
    strncpy(buffer, str.c_str(), str.length());
    char *token = strtok(buffer, ",");
    int i = 1;
    while (token != NULL) {
        brd1->set_alive_ship_count(i++, atoi(token));
        token = strtok(NULL, ",");
    }
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
    String str = "";
    while(str.substring(0, 4) != "<p1>") str = Serial.readString();
    Serial.println(F("CONFIRM P1"));
    p1 = PLAYER_TYPE(get_substring(str, "<p1>", "</p1>").toInt() - 1);

    while(str.substring(0, 4) != "<p2>") str = Serial.readString();
    Serial.println(F("CONFIRM P2"));
    p2 = PLAYER_TYPE(get_substring(str, "<p2>", "</p2>").toInt() - 1);

    while(str.substring(0, 4) != "<s1>") str = Serial.readString();
    get_ships_count_from_save(get_substring(str, "<s1>", "</s1>"), brd1);
    Serial.println(F("CONFIRM S1"));

    while(str.substring(0, 4) != "<s2>") str = Serial.readString();
    get_ships_count_from_save(get_substring(str, "<s2>", "</s2>"), brd2);
    Serial.println(F("CONFIRM S2"));

    while(str.substring(0, 4) != "<f1>") str = Serial.readString();
    Serial.println(F("CONFIRM F1"));
    memcpy(&brd1->field(0, 0), str.c_str() + 4, brd1->get_size());

    while(str.substring(0, 4) != "<f2>") str = Serial.readString();
    Serial.println(F("CONFIRM F2"));
    memcpy(&brd2->field(0, 0), str.c_str() + 4, brd2->get_size());

    while(str.substring(0, 4) != "<m1>") str = Serial.readString();
    Serial.println(F("CONFIRM M1"));
    memcpy(&brd1->mask(0, 0), str.c_str() + 4, brd1->get_size());

    while(str.substring(0, 4) != "<m2>") str = Serial.readString();
    Serial.println(F("CONFIRM M2"));
    memcpy(&brd2->mask(0, 0), str.c_str() + 4, brd2->get_size());

    while(str.substring(0, 4) != "<st>") str = Serial.readString();
    Serial.println("CONFIRM ST");
    return GAME_STATE(get_substring(str, "<st>", "</st>").toInt());
}
