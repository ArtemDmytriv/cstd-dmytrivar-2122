cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME battle_main)

set(CMAKE_ARDUINO_PATH arduino-cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_ARDUINO_PATH}/cmake/ArduinoToolchain.cmake) # Arduino Toolchain
set(CMAKE_EXTERNAL_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libraries) # arduino external libs
set(SOURCE_DIRECTORIES src)
set(INC_DIRECTORIES src)
set(BOARD_NAME uno)

set (CMAKE_CXX_STANDARD 14)
project(${PROJECT_NAME} C CXX)

# link_libraries(${ARDUINO_SDK}/libraries)

# Link external libs directories (as Arduino-IDE uses it)
link_directories(${CMAKE_EXTERNAL_LIBS_PATH})

# generate stl lib for use
set(stl_lib_SRCS ${CMAKE_EXTERNAL_LIBS_PATH}/ArduinoSTL-1.1.0/src)
set(stl_lib_HDRS ${CMAKE_EXTERNAL_LIBS_PATH}/ArduinoSTL-1.1.0/src)
set(stl_lib_BOARD ${BOARD_NAME})
generate_arduino_library(stl_lib)
list(APPEND INC_DIRECTORIES ${CMAKE_EXTERNAL_LIBS_PATH}/ArduinoSTL-1.1.0/src)

#generate xml pugixml lib for use
#set(pugixml_lib_SRCS ${CMAKE_EXTERNAL_LIBS_PATH}/pugixml-1.11.4/src)
#set(pugixml_lib_HDRS ${CMAKE_EXTERNAL_LIBS_PATH}/pugixml-1.11.4/src)
#set(pugixml_lib_BOARD ${BOARD_NAME})
#generate_arduino_library(pugixml_lib)
#list(APPEND INC_DIRECTORIES ${CMAKE_EXTERNAL_LIBS_PATH}/pugixml-1.11.4/src)

# set source files
file (GLOB Sources ${SOURCE_DIRECTORIES}/*.cpp)
# temporarly delete unneeded cli source code
list(REMOVE_ITEM Sources ${SOURCE_DIRECTORIES}/battle_cli.cpp)

include_directories(${INC_DIRECTORIES})

set(${PROJECT_NAME}_BOARD ${BOARD_NAME})
set(${PROJECT_NAME}_SRCS ${Sources})
set(${PROJECT_NAME}_PORT /dev/ttyS3)
set(${PROJECT_NAME}_LIBS stl_lib ) #pugixml_lib)

# Command to generate code arduino firmware (.hex file)
generate_arduino_firmware(${PROJECT_NAME})
