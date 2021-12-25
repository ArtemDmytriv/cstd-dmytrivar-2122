#ifndef _BATTLE_AI_H
#define _BATTLE_AI_H

#include "battle_ship.h"

class AI_BattleShip {
    int row; // prev shooten row 
    int col; // prev shooten col
    int ship_direct;
    bool need_to_finish_ship;
    long (*rand_func)();

    unsigned char *avail_moves;
    int avail_moves_count;

    SHOOT_RESULT rand_fire(BattleBoard *enemy_brd);
    SHOOT_RESULT continue_fire(BattleBoard *enemy_brd);
    void on_destroy_event(BattleBoard *enemy_brd);
public:
    AI_BattleShip(unsigned char *buffer, long (*rand_func)());
    ~AI_BattleShip();
    AI_BattleShip(const AI_BattleShip &ai) = delete;
    AI_BattleShip operator=(const AI_BattleShip &ai) = delete;

    int get_avail_moves_count() const { return avail_moves_count; }
    void init_moves(int enemy_rows, int enemy_cols);
    void reset_vars();
    SHOOT_RESULT fire(BattleBoard *enemy_brd);
};

#endif