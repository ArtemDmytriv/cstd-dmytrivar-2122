#ifndef _BATTLE_UTILS
#define _BATTLE_UTILS

#include <vector>
#include <string>

#define SYNC_MSG "#$SYNC$#"
#define END_SEQ_MSG "#$END$#"

#define SERIAL_BUFFER_LEN 128

class BattleBoard;

enum class GAME_MODES {
    player_vs_player //,
    // player_vs_ai,
    // ai_vs_ai
};

const std::vector<std::string> vec_modes = { "Player vs player" };

enum class GAME_STATE {
    wait_sync,
    main_menu,
    setup_p1_board,
    setup_p2_board,
    turns_p1,
    turns_p2,
    announce_winner,
    wait_response
};

GAME_STATE send_state(GAME_STATE new_gs);
void send_both_boards(BattleBoard *brd1, BattleBoard *brd2, bool send_mask);
int get_shot_coor(int &x, int &y);
int send_winner(int winner);

#endif
