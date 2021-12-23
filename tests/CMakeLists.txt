cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME battle_test)
set(CMAKE_CXX_STANDARD 14)

project(${PROJECT_NAME} C CXX)

message(STATUS "Fetching googletest")
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)
        
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCLI_COMPILATION")

set(SOURCE_DIRECTORIES ../src)
set(INC_DIRECTORIES ../include)
include_directories(${INC_DIRECTORIES})

file (GLOB Sources ${SOURCE_DIRECTORIES}/*.cpp)
list(REMOVE_ITEM Sources ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIRECTORIES}/battle_utils.cpp)
list(REMOVE_ITEM Sources ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIRECTORIES}/battle_main.cpp)
message(${Sources})

add_executable(unit_test main_test.cpp ${Sources})

target_link_libraries(unit_test gtest gtest_main)
