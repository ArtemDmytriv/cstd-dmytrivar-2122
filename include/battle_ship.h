#ifndef _BATTLE_SHIP_H
#define _BATTLE_SHIP_H

#include <vector>
#include <map>
#include <utility>

#define SHIP_MAX_SIZE 4
#define SHIP_MIN_SIZE 1

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

// typedef struct {
//     // represents 2d square array of field, board with own ships
//     char **field;
//     // represents 2d square array of field, will shared to the opponent
//     char **mask;
//     // number row/col of the board (size x size)
//     uint8_t size;
//     uint8_t ship_counts[SHIP_MAX_SIZE + 1]; // total_ships, 1, 2, 3, 4;
// } battle_board;

// typedef struct {
//     uint8_t x, y;
//     uint8_t size;
//     bool is_horizontal;
// } ship;

struct Ship {
    int x, y;
    int size;
    bool is_horizontal;
};

class BattleBoard {
private:
    // represents 2d square array of field, board with own ships
    char **_field;
    // represents 2d square array of field, will shared to the opponent
    char **_mask;
    // number rows, col of the board
    int rows,
        cols;

    int total_ship_count;
    std::map<int, int> ship_counts;
    
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
        return ship_counts.find(ship_size) != ship_counts.end()? ship_counts[ship_size] : -1;
    };
    int get_total_alive_count() const {
        return total_ship_count;
    }
    char mask(int r, int c) const { 
        return _mask[r][c];
    }
    char field(int r, int c) const {
        return _field[r][c];
    }
    
    void clear(char fill_char = ' ');
    void fill_both(int r, int c, char ch) {
        mask(r, c) = ch;
        field(r, c) = ch;
    }
    char& mask(int r, int c) {
        return _mask[r][c];
    }
    char& field(int r, int c) { 
        return _field[r][c]; 
    }
    // ================ Game Behavior methods ================
    int board_set_ship(Ship sh);
    int board_set_rand_ships(const std::vector<std::pair<int, int>> &ship_gen_counts);

    SHOOT_RESULT board_fire_at(int row, int col);
};

#ifdef CLI_COMPILATION
void game_loop(BattleBoard &brd1, PLAYER_TYPE pt1, BattleBoard &brd2, PLAYER_TYPE pt2);
#endif

#endif