#ifndef _BATTLE_SHIP_H
#define _BATTLE_SHIP_H

enum class PLAYER_TYPE{
    HUMAN_PLAYER_TYPE,
    AI_PLAYER_TYPE
};

enum class SHOOT_RESULT {
    NOT_ALLOWED,
    MISSED_HIT,
    SUCCESS_HIT,
    SUCCESS_FINISH_HIT
};

struct Ship {
    int x, y;
    int size;
    bool is_horizontal;
};

class BattleBoard {
private:
    // represents 2d square array of field, board with own ships
    char *_field;
    // represents 2d square array of field, will shared to the opponent
    char *_mask;
    // number rows, col of the board
    int rows,
        cols;

    int total_ship_count;
    int ship_counts[4];
    
    // ================ Private methods ================
    void board_event_destroyed(int row, int col, bool is_horizontal);
    SHOOT_RESULT board_check_ship_destroyed(int row, int col);
public:
    BattleBoard(int rows, int cols, char fill_char = ' ');
    ~BattleBoard();
    BattleBoard(const BattleBoard &brd) = delete;
    BattleBoard operator=(const BattleBoard &brd) = delete;

    int get_size() const { 
        return cols * rows; 
    }
    int get_rows() const { 
        return rows; 
    }
    int get_cols() const {
        return cols;
    }

    int get_alive_count(int ship_size) { 
        return ship_counts[ship_size - 1];
    };
    void set_alive_ship_count(int ship_size, int count) {
        ship_counts[ship_size - 1] = count;
    }
    int get_total_alive_count() const {
        return total_ship_count;
    }
    char mask(int r, int c) const { 
        return _mask[r * cols + c];
    }
    char field(int r, int c) const {
        return _field[r * cols + c];
    }
    
    void clear(char fill_char = ' ');
    void fill_both(int r, int c, char ch) {
        mask(r, c) = ch;
        field(r, c) = ch;
    }
    char& mask(int r, int c) {
        return _mask[r * cols + c];
    }
    char& field(int r, int c) { 
        return _field[r * cols + c]; 
    }
    // ================ Game Behavior methods ================
    int board_set_ship(Ship sh);
    int board_set_rand_ships(const int ship_gen_counts[4], long (*rand_func)());

    SHOOT_RESULT board_fire_at(int row, int col);
};

#ifdef CLI_COMPILATION
void game_loop(BattleBoard &brd1, PLAYER_TYPE pt1, BattleBoard &brd2, PLAYER_TYPE pt2);
#endif

#endif