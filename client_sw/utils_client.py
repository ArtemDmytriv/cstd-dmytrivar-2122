import serial
from enum import Enum
import string
import re
import os

SYNC_MSG = "#$SYNC$#"
END_SEQ_MSG = "#$END$#"

cells = {   '#' : "###",
            'X' : "+X+",
            ' ' : "   ",
            '0' : " O "}

class State(Enum):
    wait_sync = 0
    main_menu = 1
    setup_p1_board = 2
    setup_p2_board = 3 
    turns_p1 = 4
    turns_p2 = 5
    turns_AI1 = 6
    turns_AI2 = 7
    announce_winner = 8
    wait_response = 9

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

def print_menu(menu_types : list):
    print("==== Game Modes ====")
    for i, mode in enumerate(menu_types):
        print(i + 1, ") ", mode, sep="")
    print("\n====*====*=====*====")

def get_menu_mode(menu_types : list) -> int:
    menu_types = list(filter(None, menu_types[:-1]))
    while(True):
        print_menu(menu_types)
        try:
            mode = int(input("Enter mode number: "))
            if (mode - 1 < len(menu_types) and mode > 0):
                break    
        except:
            print("Bad input, try again")
    return mode

def get_board_serial(arduino):
    brd_temp = []
    brd_msg = arduino.readline().decode("utf-8")[:-2]
    while (brd_msg != END_SEQ_MSG):
        brd_msg = arduino.readline().decode("utf-8")[:-2]
        brd_temp.append(brd_msg)
    return next(filter(lambda x: 'b' in x and 'e' in x, brd_temp))

def print_board(brd : str):
    brd = brd[1:-1]
    print()
    print("   ", end="")
    for ch in string.ascii_uppercase[:10]:
        print("| ", ch, " ", sep="", end="")
    print()
    for r in range(10):
        print("-"*11*4 + "\n{:<3}".format(r+1), end="")
        for val in brd[r * 10: r * 10 + 10]:
            print("|", cells[val], sep="", end="")
        print("|")
    print("-"*11*4 + '\n')

def print_battleground(brd1 : str, brd2 : str, arrow : str):
    brd1 = brd1[1:-1]
    brd2 = brd2[1:-1]
    os.system("clear")
    print("{} {} {}".format("Player1".center(40), arrow, "Player2".center(40)))
    print()
    print("   ", end="")
    for ch in string.ascii_uppercase[:10]:
        print("| ", ch, " ", sep="", end="")
    print("|&&&", end="")
    for ch in string.ascii_uppercase[:10]:
        print("| ", ch, " ", sep="", end="")
    
    print("|")
    for r in range(10):
        print("-"*11*4 + "&&&" + "-"*10*4 + "\n{:<3}".format(r+1), end="")
        for val in brd1[r * 10: r * 10 + 10]:
            print("|", cells[val], sep="", end="")
        print("|&&&", end="")
        for val in brd2[r * 10: r * 10 + 10]:
            print("|", cells[val], sep="", end="")
        print("|")
    print("-"*11*4 + "&&&" + "-"*10*4 + '\n')

def get_user_shot_cell():
    x, y = None, None
    cell = input("Enter cell for shoot (format example A5 or 5A): ")
    try:
        for elem in list(filter(None, re.split('(\d+)', cell)))[:2]:
            if (elem in string.ascii_letters):
                y = string.ascii_uppercase.index(elem.upper())
            else:
                x = int(elem) - 1
        if (x >= 0 and x < 10 and y >= 0 and y < 10):
            return x, y
    except:
        return None, None
    return None, None

def send_user_shot_cell(arduino, x, y):
    arduino.write("s>{} {}".format(x, y).encode("utf-8"))
    return 0

#s>1 0