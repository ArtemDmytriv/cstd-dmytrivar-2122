Student: Dmytriv Artem
Group: CE-47
Subject: CSTD

Project name: Sea Battle game
Programming language: C, C++

Additional tools: Bash, Makefile
HW Interface: UART
Data driven format: XML

Play modes: Man vs Man, Man vs AI

Actions: New, Recover, Restart
The proposal for data driver approuch: game states, ship configurations, AI parameters.

Use ci.sh script or do Requirements step by step 

Requirements:
1) Clone arduino-cmake scripts
    git clone https://github.com/queezythegreat/arduino-cmake arduino-cmake
2) Get STL library
    cd libraries
    wget https://downloads.arduino.cc/libraries/github.com/mike-matera/ArduinoSTL-1.1.0.zip
    unzip ArduinoSTL-1.1.0.zip
    rm ArduinoSTL-1.1.0.zip
3) Get XML library
    wget https://github.com/zeux/pugixml/releases/download/v1.11.4/pugixml-1.11.4.tar.gz
    tar zxf pugixml-1.11.4.tar.gz
    rm pugixml-1.11.4.tar.gz
4) Create build dir and run cmake
    cd ..
    mkdir -p build
    cd build && cmake -DARDUINO_PORT_PATH={YOUR COM PORT WITH arduino} ..
5) Make and upload project
    make upload