#ifndef _BATTLE_UTILS
#define _BATTLE_UTILS

#define SYNC_MSG "#$SYNC$#"
#define CONFIRM_MSG "#$CONFIRM$#"
#define END_SEQ_MSG "#$END$#"

#define BOARD_SIZE 10

#include "battle_ship.h"

class String;

enum class PLAYER_TYPE;

enum class GAME_MODES {
    player_vs_player ,
    player_vs_ai,
    ai_vs_ai
};

enum class GAME_MENU {
    new_game,
    load_game
};

enum class GAME_STATE {
    wait_sync,
    main_menu,
    mode_menu,
    load_game,
    setup_p1_board,
    setup_p2_board,
    turns_p1,
    turns_p2,
    turns_AI1,
    turns_AI2,
    save_game,
    announce_winner,
    wait_response
};

GAME_STATE send_state(GAME_STATE new_gs);
void send_both_boards(BattleBoard *brd1, BattleBoard *brd2, bool send_mask);
int get_shot_coor(int &x, int &y);
int send_winner(int winner);

void write_xml_save(BattleBoard *brd1, PLAYER_TYPE &p1);
void send_ships_count(BattleBoard *brd1);

GAME_STATE load_save(BattleBoard *brd1, PLAYER_TYPE &p1, BattleBoard *brd2, PLAYER_TYPE &p2);
#endif
