import serial
import sys

com_port = sys.argv[1]

arduino = serial.Serial(port=com_port, baudrate=9600, timeout=1)

while (1):
    hw_msg = arduino.readline()
    print(hw_msg)