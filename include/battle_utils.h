#ifndef _BATTLE_UTILS
#define _BATTLE_UTILS

#include <vector>
#include <string>

#define SYNC_MSG "#$SYNC$#"
#define END_SEQ_MSG "#$END$#"

class BattleBoard;
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

const std::vector<std::string> vec_menu = { "New Game", "Load Game (WIP)"};
const std::vector<std::string> vec_modes = { "Player vs player", "Player vs AI", "AI vs AI" };

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
    announce_winner,
    wait_response
};

GAME_STATE send_state(GAME_STATE new_gs);
void send_both_boards(BattleBoard *brd1, BattleBoard *brd2, bool send_mask);
int get_shot_coor(int &x, int &y);
int send_winner(int winner);

std::string read_xml_save();
void write_xml_save(BattleBoard *brd1, PLAYER_TYPE &p1);

GAME_STATE load_save(BattleBoard *brd1, PLAYER_TYPE &p1, BattleBoard *brd2, PLAYER_TYPE &p2);
GAME_STATE parse_save_from_serial(const String &str, BattleBoard *brd1, PLAYER_TYPE &p1, BattleBoard *brd2, PLAYER_TYPE &p2);

#endif
