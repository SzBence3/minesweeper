cmake_minimum_required(VERSION 3.23)

project(Minesweeper)

add_executable(minesweeper
    src/main.cpp
    src/Game.cpp
    src/Screen.cpp
    src/GameHandler.cpp
    src/Config.cpp
    src/Config.h
    src/Game.h
    src/GameHandler.h
    src/Screen.h
    src/utils.h
    src/utils.cpp
)

