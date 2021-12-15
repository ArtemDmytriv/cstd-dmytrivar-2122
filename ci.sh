#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Must provide COM port path"
    echo "Example: $0 /dev/ttyS3"
    exit
fi

## Clone arduino-cmake toolchain if it not exist
if [ ! -d "arduino-cmake" ]; then
    git clone https://github.com/queezythegreat/arduino-cmake arduino-cmake
fi

run_install()
{
    read -p "Do you want to install missing libraries? [Y/n]: " answer
    answer=${answer:Y}
    [[ $answer =~ [Yy] ]] && apt-get install ${boostlibnames[@]}
}

## Run the run_install function if sany of the libraries are missing
reqpkgs=("cmake" "arduino-mk")
dpkg -s "${reqpkgs[@]}" >/dev/null 2>&1 || run_install

## Install static libraries if it not exist
mkdir -p libraries && cd libraries
if [ ! -d "ArduinoSTL-1.1.0" ]; then
    wget https://downloads.arduino.cc/libraries/github.com/mike-matera/ArduinoSTL-1.1.0.zip
    unzip ArduinoSTL-1.1.0.zip
    rm ArduinoSTL-1.1.0.zip
fi
# if [ ! -d "pugixml-1.11.4" ]; then
#     wget https://github.com/zeux/pugixml/releases/download/v1.11.4/pugixml-1.11.4.tar.gz
#     tar zxf pugixml-1.11.4.tar.gz
#     rm pugixml-1.11.4.tar.gz
# fi
cd ..

## Run Cmake
mkdir -p build
cd build && rm -rf ** && cmake -DARDUINO_PORT_PATH=$1 ..

## Run make upload
make upload
