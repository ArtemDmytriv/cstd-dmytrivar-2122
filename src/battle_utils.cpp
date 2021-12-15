#include "battle_utils.h"

#include <Arduino.h>

char buffer[SERIAL_BUFFER_LEN];

GAME_STATE send_state(GAME_STATE gs, GAME_STATE new_gs) {
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

int get_shot_coor(int &x, int &y) {
    
}
