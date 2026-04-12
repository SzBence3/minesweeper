#pragma once
#include "Screen.h"
#include <utility>
#include <vector>

namespace tms{

    struct Cell{
        bool isMine : 1 ;
        bool isRevealed : 1 ;
        bool isFlagged : 1 ;
        Cell() : isMine(false), isRevealed(false), isFlagged(false) {}
    };

    enum GameState{
        RUNNING,
        STARTING,
        WON = -1,
        LOST = -2,
    };
    /** Minesweeper game object */
    class Game{
        int height, width;
        int cursorX, cursorY;
        int flagsLeft;
        bool isFastSweep : 1;
        bool isFastFlag : 1;
        GameState state;
        std::vector<std::vector<Cell>>cells;

        void startGame(int x, int y);
        std::vector<std::vector<Cell>> getCellsFromImage(const std::string& data, int height, int width);
        GameState calcGameState();
        int countFlagsLeft();
        int countAdjacentMines(int x, int y);
        int countAdjacentFlags(int x, int y);
        void sweepAdjacent(int x, int y);
        int countAdjacentRevealed(int x, int y);
        int countAdjacentUnRevealed(int x, int y);
        void flagAdjacent(int x, int y);

      public:
        struct Image{
            int height, width;
            bool isFastSweep : 1;
            bool isFastFlag : 1;
            int minecount;
            std::string data;
        };
        Game(Image image);
        bool sweep(int x, int y);
        bool toggleFlag(int x, int y);
        GameState getState(){
            return state;
        }
        Picture getPicture();
        Image getImage();
        std::pair<int, int> getCursorPosition(){
            return {cursorX, cursorY};
        }
        std::pair<int, int> getSize(){
            return {height, width};
        }
        int getFlagsLeft(){
            return flagsLeft;
        }
        int setCursorPosition(int x, int y){
            if(x < 0 || x >= width || y < 0 || y >= height) return -1;
            cursorX = x;
            cursorY = y;
            return 0;
        }
    };
}
