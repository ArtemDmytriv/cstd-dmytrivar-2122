import mysql.connector
import xml.etree.ElementTree as ET

def save_game_to_db(filename):
    mydb = mysql.connector.connect(
        host="localhost",
        user="artem-wsl",
        password="qwer1234",
        database="db_ship_wars"
    )

    mycursor = mydb.cursor()

    data_to_parse = filename
    xml_tree = ET.parse(data_to_parse)
    root = xml_tree.getroot()

    ship_values =  { 
    'player 1 type' : None,
    'player 2 type' : None,
    'ships 1' : None, 
    'ships 2' : None,
    'field 1' : None,
    'field 2' : None,
    'mask 1' : None,
    'mask 2' : None
    }

    ship_values['player 1 type'] = int(root.find('p1').text)
    ship_values['player 2 type'] = int(root.find('p2').text)
    ship_values['ships 1'] = root.find('s1').text
    ship_values['ships 2'] = root.find('s2').text
    ship_values['field 1'] = root.find('f1').text
    ship_values['field 2'] = root.find('f2').text
    ship_values['mask 1'] = root.find('m1').text
    ship_values['mask 2'] = root.find('m2').text

    create_table = '''
    create table if not exists ship_state (
        player_1_type INT, 
        player_2_type INT, 
        ships_1 VARCHAR(20),
        ships_2 VARCHAR(20),
        field_1 VARCHAR(100),
        field_2 VARCHAR(100),
        mask_1 VARCHAR(100),
        mask_2 VARCHAR(100)
    );'''

    mycursor.execute(create_table)

    insert = "INSERT INTO ship_state (player_1_type, player_2_type, ships_1, ships_2, field_1, field_2, mask_1, mask_2) VALUES (%s, %s, %s, %s, %s, %s, %s, %s)"
    val = tuple(ship_values.values())
    print(len(val), val)
    mycursor.execute(insert, val) 

    mydb.commit()