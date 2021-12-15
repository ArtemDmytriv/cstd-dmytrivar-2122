import serial
import sys
from utils_client import *

from serial.serialposix import Serial


com_port = "/dev/ttyS3"

arduino = serial.Serial(port=com_port, baudrate=9600, timeout=1)

current_state = State.wait_sync

menu_types = []
brd_1_full = None
brd_2_full = None
brd_1_mask = None
brd_2_mask = None

def get_state(arduino) -> State:
    msg = str()
    while(True):
        msg = arduino.readline().decode("utf-8")[:-2]
        try: 
            if (msg == SYNC_MSG):
                raise
            #print("Debug: ", msg)
            msg = int(msg)
            print("Current State: ", State(msg))
            arduino.write(str(msg).encode("utf-8"))
            break
        except:
            continue
    return State(msg)

while (True):
    if (current_state == State.wait_sync):
        hw_msg = arduino.readline().decode("utf-8")[:-2]
        if (hw_msg == SYNC_MSG):
            arduino.write(b"#")
            current_state = get_state(arduino)
    elif (current_state == State.main_menu):
        mm_msg = str()
        menu_types.clear()
        while (mm_msg != END_SEQ_MSG):
            mm_msg = arduino.readline().decode("utf-8")[:-2]
            menu_types.append(mm_msg)
        # print menu
        mode = get_menu_mode(menu_types)
        arduino.write("m{}".format(mode).encode("utf-8"))
        # get next state
        current_state = get_state(arduino)
    elif (current_state == State.setup_p1_board):
        brd_1_full = get_board_serial(arduino)
        print_board(brd_1_full)

        current_state = get_state(arduino)
    elif (current_state == State.setup_p2_board):
        brd_2_full = get_board_serial(arduino)
        print_board(brd_2_full)

        current_state = get_state(arduino)
    elif (current_state == State.turns_p1):
        brd_2_mask = get_board_serial(arduino)
        print_board(brd_2_mask)
        # shoot
        x = None
        while (not x):
            x, y = get_user_shot_cell()
        send_user_shot_cell(arduino, x, y)
        # get next state
        current_state = get_state(arduino)
    elif (current_state == State.turns_p2):
        brd_1_mask = get_board_serial(arduino)
        print_board(brd_1_mask)
        # shoot
        x = None
        while (not x):
            x, y = get_user_shot_cell()
        send_user_shot_cell(arduino, x, y)
        # get next state
        current_state = get_state(arduino)