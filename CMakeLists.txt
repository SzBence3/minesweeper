cmake_minimum_required(VERSION 3.23)

project(Minesweeper)

# setting c++ version to 20
set(CMAKE_CXX_STANDARD 20)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(term-minesweeper
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
    src/Generation.cpp
    src/solve.cpp
)

install(
    TARGETS term-minesweeper
    DESTINATION bin
)
