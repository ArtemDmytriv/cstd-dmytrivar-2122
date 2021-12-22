#include "gtest/gtest.h"

#include "battle_ship.h"

#define BOARD_SIZE 10

class TestBattleBoard : public ::testing::Test {
protected:
    void SetUp() {
        brd = new BattleBoard(BOARD_SIZE, BOARD_SIZE);
    }
    void TearDown() {
        delete brd;
    }
    BattleBoard *brd;
};

class TestBattleBoard_with_Ships : public ::testing::Test {
protected:
    void SetUp() {
        brd = new BattleBoard(BOARD_SIZE, BOARD_SIZE);
        brd->board_set_ship(Ship{0, 0, 4, true});
        brd->board_set_ship(Ship{2, 2, 1, true});
        brd->board_set_ship(Ship{4, 6, 2, false});
        brd->board_set_ship(Ship{6, 0, 3, false});
    }
    void TearDown() {
        delete brd;
    }
    BattleBoard *brd;
};


TEST(BattleBoardCreate, testInit) {
    auto brd1 = new BattleBoard(4, 4);
    auto brd2 = new BattleBoard(4, 8);
    auto brd3 = new BattleBoard(BOARD_SIZE, BOARD_SIZE);

    ASSERT_EQ(brd1->get_rows(), 4);
    ASSERT_EQ(brd1->get_cols(), 4);
    ASSERT_EQ(brd1->get_size(), 4*4);

    ASSERT_EQ(brd2->get_rows(), 4);
    ASSERT_EQ(brd2->get_cols(), 8);
    ASSERT_EQ(brd2->get_size(), 4*8);

    ASSERT_EQ(brd3->get_rows(), BOARD_SIZE);
    ASSERT_EQ(brd3->get_cols(), BOARD_SIZE);
    ASSERT_EQ(brd3->get_size(), BOARD_SIZE*BOARD_SIZE);

    ASSERT_EXIT( BattleBoard(-1, 5), testing::ExitedWithCode(2), "Incorrect num of cols or rows");
    delete brd3;
    delete brd2;
    delete brd1;
}

TEST_F(TestBattleBoard, testClear) {
    brd->fill_both(0, 0, '#');
    brd->fill_both(0, 2, '&');

    std::string expected (brd->get_size(), ' ');
    expected[0] = '#'; expected[2] = '&';
    ASSERT_EQ( std::string(&brd->field(0,0), &brd->field(0,0) + brd->get_size()) , expected);
    ASSERT_EQ( std::string(&brd->mask(0,0), &brd->mask(0,0) + brd->get_size())   , expected);
    
    brd->clear();

    expected = std::string(brd->get_size(), ' ');
    ASSERT_EQ( std::string(&brd->field(0,0), &brd->field(0,0) + brd->get_size()) , expected);
    ASSERT_EQ( std::string(&brd->mask(0,0), &brd->mask(0,0) + brd->get_size())   , expected);
}

TEST_F(TestBattleBoard, testManualSetShip) {
    Ship sh4 {0, 0, 4, true}, // correct
        sh2_f {1, 4, 2, true}, // noncorrect diag from sh4
        sh3 {6, 5, 3, false}, // correct
        sh2 {5, 7, 2, false}, // correct
        sh4_f {9, 9, 4, true}; // noncorrect 
    
    ASSERT_EQ(brd->board_set_ship(sh4), 0);
    ASSERT_EQ(brd->board_set_ship(sh2_f), -1);
    ASSERT_EQ(brd->board_set_ship(sh3), 0);
    ASSERT_EQ(brd->board_set_ship(sh2), 0);
    ASSERT_EQ(brd->board_set_ship(sh4_f), -1);

    ASSERT_EQ(brd->field(0,0), '#');
    ASSERT_EQ(brd->field(0,3), '#');
}

TEST_F(TestBattleBoard, testRandomSetShips) {
    std::vector<std::pair<int, int>> ship_vec { {4, 1}, {3, 2}, {2, 3}, {1, 4}};
    brd->board_set_rand_ships(ship_vec, []() -> long { return rand(); });

    ASSERT_EQ(brd->get_total_alive_count(), 10);
    ASSERT_EQ(brd->get_alive_count(4), 1);
    ASSERT_EQ(brd->get_alive_count(3), 2);
    ASSERT_EQ(brd->get_alive_count(2), 3);
    ASSERT_EQ(brd->get_alive_count(1), 4);

    brd->clear();
}

TEST_F(TestBattleBoard_with_Ships, testShoot) {
    ASSERT_EQ(brd->mask(1,1), ' ');
    ASSERT_EQ(brd->board_fire_at(1,1), SHOOT_RESULT::MISSED_HIT);
    ASSERT_EQ(brd->mask(1,1), '0');
    ASSERT_EQ(brd->board_fire_at(1,1), SHOOT_RESULT::NOT_ALLOWED);

    ASSERT_EQ(brd->board_fire_at(0,0), SHOOT_RESULT::SUCCESS_HIT);
    ASSERT_EQ(brd->mask(0,0), 'X');

    ASSERT_EQ(brd->board_fire_at(0,1), SHOOT_RESULT::SUCCESS_HIT);
    ASSERT_EQ(brd->mask(0,1), 'X');

    ASSERT_EQ(brd->board_fire_at(0,1), SHOOT_RESULT::NOT_ALLOWED);
}

TEST_F(TestBattleBoard_with_Ships, testFinishShoot) {
    brd->board_fire_at(0,0);
    brd->board_fire_at(0,1);
    brd->board_fire_at(0,2);

    ASSERT_EQ(brd->get_alive_count(4), 1);
    ASSERT_EQ(brd->board_fire_at(0,3), SHOOT_RESULT::SUCCESS_FINISH_HIT);
    ASSERT_EQ(brd->get_alive_count(4), 0);

    ASSERT_EQ(brd->board_fire_at(2,2), SHOOT_RESULT::SUCCESS_FINISH_HIT);
    ASSERT_EQ(brd->get_alive_count(1), 0);
}

TEST_F(TestBattleBoard_with_Ships, testDestroyed_at_middle) {
    brd->board_fire_at(2,2); // destroy 1-size ship
    ASSERT_EQ(std::string(&brd->field(1,1), &brd->field(1,1) + 3), "000"); // check up
    ASSERT_EQ(std::string(&brd->field(2,1), &brd->field(2,1) + 3), "0X0"); // check middle
    ASSERT_EQ(std::string(&brd->field(3,1), &brd->field(3,1) + 3), "000"); // check down
}

TEST_F(TestBattleBoard_with_Ships, testDestroyed_at_edge) {
    brd->board_fire_at(6,0); // shoot 3-size ship
    brd->board_fire_at(7,0); // shoot 3-size ship
    brd->board_fire_at(8,0); // destroy 3-size ship
    ASSERT_EQ(std::string(&brd->field(5,0), &brd->field(5,0) + 2), "00"); // check up
    ASSERT_EQ(std::string(&brd->field(6,0), &brd->field(6,0) + 2), "X0"); // 
    ASSERT_EQ(std::string(&brd->field(7,0), &brd->field(7,0) + 2), "X0"); // 
    ASSERT_EQ(std::string(&brd->field(8,0), &brd->field(8,0) + 2), "X0"); // 
    ASSERT_EQ(std::string(&brd->field(9,0), &brd->field(9,0) + 2), "00"); // check down
}

TEST_F(TestBattleBoard_with_Ships, testDestroyed_at_corner) {
    brd->board_fire_at(0,0); // shoot 3-size ship
    brd->board_fire_at(0,1); // shoot 3-size ship
    brd->board_fire_at(0,2); // destroy 3-size ship
    brd->board_fire_at(0,3); // destroy 3-size ship
    ASSERT_EQ(std::string(&brd->field(0,0), &brd->field(0,0) + 5), "XXXX0"); //
    ASSERT_EQ(std::string(&brd->field(1,0), &brd->field(1,0) + 5), "00000"); // 
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
