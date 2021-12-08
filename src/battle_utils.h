#ifndef _BATTLE_UTILS
#define _BATTLE_UTILS

enum class GAME_MODES {
    player_vs_player //,
    // player_vs_ai,
    // ai_vs_ai
};

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


#endif