import serial
import sys
from utils_client import *

from serial.serialposix import Serial


com_port = "/dev/ttyS3"

arduino = serial.Serial(port=com_port, baudrate=115200, timeout=.1)

current_state = State.wait_sync

menu_types = []
brd_1_full = None
brd_2_full = None
brd_1_mask = 'b' + ' '*100 + 'e'
brd_2_mask = 'b' + ' '*100 + 'e'

while (True):
    if (current_state == State.wait_sync):
        hw_msg = arduino.readline().decode("utf-8")[:-2]
        if (hw_msg == SYNC_MSG):
            arduino.write(b"#")
            current_state = get_state(arduino)
        continue
    elif (current_state == State.main_menu or current_state == State.mode_menu):
        mm_msg = str()
        menu_types.clear()
        while (mm_msg != END_SEQ_MSG):
            mm_msg = arduino.readline().decode("utf-8")[:-2]
            menu_types.append(mm_msg)
        # print menu
        mode = get_menu_mode(menu_types)
        if (current_state == State.main_menu):
            arduino.write("t{}".format(mode).encode("utf-8"))
        else:
            arduino.write("m{}".format(mode).encode("utf-8"))
    elif (current_state == State.load_game):
        send_saved_game(arduino)
    elif (current_state == State.setup_p1_board):
        brd_1_full = get_board_serial(arduino)
        print_board(brd_1_full)
    elif (current_state == State.setup_p2_board):
        brd_2_full = get_board_serial(arduino)
        print_board(brd_2_full)
    elif (current_state == State.turns_p1):
        brd_1_mask = get_board_serial(arduino)
        brd_2_mask = get_board_serial(arduino)
        print_battleground(brd_1_mask, brd_2_mask, "-->")
        # shoot
        x = y = None
        while (x == None or y == None):
            x, y = get_user_shot_cell()
        if (x < 0):
            arduino.write("save".encode("utf-8"))
        else:
            send_user_shot_cell(arduino, x, y)
        # get next state
    elif (current_state == State.turns_p2):
        brd_1_mask = get_board_serial(arduino)
        brd_2_mask = get_board_serial(arduino)
        print_battleground(brd_1_mask, brd_2_mask, "<--")
        # shoot
        x = -1
        while (not x or x < 0):
            x, y = get_user_shot_cell()
        if (x < 0):
            arduino.write("save".encode("utf-8"))
        else:
            send_user_shot_cell(arduino, x, y)
        # get next state
    elif (current_state == State.turns_AI1):
        brd_1_mask = get_board_serial(arduino)
        brd_2_mask = get_board_serial(arduino)

        print_battleground(brd_1_mask, brd_2_mask, "--AI->")
    elif (current_state == State.turns_AI2):
        brd_1_mask = get_board_serial(arduino)
        brd_2_mask = get_board_serial(arduino)

        print_battleground(brd_1_mask, brd_2_mask, "<-AI--")
    elif (current_state == State.save_game):
        recv_save_game(arduino)
        current_state = State.wait_sync
        print("Save succeed")
        continue
    elif (current_state == State.announce_winner):
        brd_1_full = get_board_serial(arduino)
        brd_2_full = get_board_serial(arduino)
        winner = str()
        while (not 'w' in winner):
            winner = arduino.readline().decode("utf-8")[:-2]
        if (len(winner) == 2):
            if (winner[1] == '1'):
                print_battleground(brd_1_full, brd_2_full, "Player 1 WON!")
            else:
                print_battleground(brd_1_full, brd_2_full, "Player 2 WON!")
    current_state = get_state(arduino)   