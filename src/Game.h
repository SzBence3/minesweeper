#pragma once
#include "Screen.h"
#include <utility>
#include <vector>
#include "Colors.h"

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

        /** Start the game, reveal the cell at (x, y)*/
        void startGame(int x, int y);
        /** Get the cells from the image data */
        std::vector<std::vector<Cell>> getCellsFromImage(const std::string& data, int height, int width);
        /** Calculate the current game state */
        GameState calcGameState();
        /** Count the number of mines left */
        int countFlagsLeft();
        /** Count the number of adjacent mines to the cell at (x, y) */
        int countAdjacentMines(int x, int y);
        /** Count the number of adjacent flags to the cell at (x, y) */
        int countAdjacentFlags(int x, int y);
        /** Sweep the uflagged adjacent cells to the cell at (x, y) */
        void sweepAdjacent(int x, int y);
        /** Count the number of adjacent revealed cells to the cell at (x, y) */
        int countAdjacentRevealed(int x, int y);
        /** Count the number of adjacent unrevealed cells to the cell at (x, y) */
        int countAdjacentUnRevealed(int x, int y);
        /** Flag the adjacentunrevealed cells to the cell at (x, y) */
        void flagAdjacent(int x, int y);

        struct SolveCell{
            int x,y;
            std::vector<int> groups;
        };
        struct Group{
            std::vector<int> cellIndices;
            int minMines, maxMines; 
            bool operator==(const Group& other) const{
                return cellIndices == other.cellIndices && minMines == other.minMines && maxMines == other.maxMines;
            }
        };
        /** Get the cells that are not revealed */
        std::vector<SolveCell> getSolveCells();
        /** Get the groups of cells that are adjacent to the same revealed cell and modify the cells so they know which group are they in.*/
        std::vector<Group> getGroups(std::vector<SolveCell>& cells);
        /** Make the moves where in a group the number of cells equals to the minimum numbers of mines or the maximum number of mines is zero*/
        bool makeObviusMoves(std::vector<SolveCell>& cells, std::vector<Group>& groups);
        /** Create new groups for each overlapping group. */
        bool makeNewGroups(std::vector<SolveCell>& cells, std::vector<Group>& groups);
        /** Add a group with the number of mines left and all unrevealed cells as members. */
        void addMineCountGroup(std::vector<SolveCell>& cells, std::vector<Group>& groups);
        /** Make 3 new groups from the overlap of two groups. */
        void makeNewGroupsFromOverlap(std::vector<SolveCell>& cells, std::vector<Group>& groups, int i, int j);
        /** sort and merge the groups with the same cells. */
        void mergeIdenticalGroups(std::vector<SolveCell>& cells, std::vector<Group>& groups);
        /** Reassign the groups indices of the cells after merging groups. */
        void fixCellGroups(std::vector<SolveCell>& cells, const std::vector<Group>& groups); 
            /** Debug function to print the groups and their cells. */
        void debugPrintGroups(const std::vector<SolveCell>& cells, const std::vector<Group>& groups);

        std::string to_string(Group& group, const std::vector<SolveCell>& cells);


        void generateMines(int x, int y);
        bool isSolvable();
        void reset();

      public:
        struct Settings{
            bool isFastSweep = true;
            bool isFastFlag = true;
            Color cursorBackgroundColor = YELLOW_BG;
            Color flagColor = RED_FG;
            Color flagBackgroundColor = DEFAULT_BG;
            Color unrevealedColor = BRIGHT_BLACK_BG;
            Color frameColor = WHITE_FG;
            Color frameBackgroundColor = DEFAULT_BG;
            Color numberColors[9] = {DEFAULT_FG, BRIGHT_BLUE_FG, BRIGHT_GREEN_FG, BRIGHT_RED_FG, BRIGHT_MAGENTA_FG, BRIGHT_CYAN_FG, BRIGHT_YELLOW_FG, BRIGHT_WHITE_FG, BRIGHT_WHITE_FG};
            Color numberBackgroundColor = DEFAULT_BG;
            Color mineColor = RED_FG;
            Color mineBackgroundColor = DEFAULT_BG;
            std::string flagSymbol = "";
            std::string mineSymbol = "󰷚";
            char upKey = 'k', downKey='j', leftKey = 'h', rightKey = 'l', sweepKey = 's', flagKey = 'f';
        };
      private:
        Settings settings;
      public:
        struct Image{
            int height, width;
            int minecount;
            std::string data;
            Settings settings;
        };

        Game(Image image);
        /** Sweep the cell at (x, y) and return true if it was successful. */
        bool sweep(int x, int y);
        /** Flag the cell at (x, y) and return true if it was successful. */
        bool toggleFlag(int x, int y);
        bool setFlag(int x, int y, bool flag);
        GameState getState(){
            return state;
        }
        /** Get the picture of the current game state. */
        Picture getPicture();
        /** Get the image of the current game state. */
        Image getImage();
        /** Get the cursor position. */
        std::pair<int, int> getCursorPosition(){
            return {cursorX, cursorY};
        }
        /** Get the size of the board of the game. */
        std::pair<int, int> getSize(){
            return {height, width};
        }
        /** Get the number of flags left. */
        int getFlagsLeft(){
            return flagsLeft;
        }
        /** Set the cursor position to (x, y) and return true if it was successful. */
        int setCursorPosition(int x, int y){
            if(x < 0 || x >= width || y < 0 || y >= height) return -1;
            cursorX = x;
            cursorY = y;
            return 0;
        }
        /** Get the settings of the game. */
        Settings getSettings(){
            return settings;
        }
        /** Make a solve iteration and return true if any move was made. */
        bool solveIteration();
        /** Solve the game using logic. */
        void solve();
    };
}
