#include "Game.h"
#include "Colors.h"
#include <algorithm>
#include <cstring>
#include <random>

namespace tms{
    Game::Game(Game::Image image) 
    {
        width = image.width;
        height = image.height;
        isFastFlag = image.settings.isFastFlag;
        isFastSweep = image.settings.isFastSweep;

        cells = getCellsFromImage(image.data, height, width);
        state = calcGameState();
        flagsLeft = countFlagsLeft();
        if(state == STARTING) flagsLeft = image.minecount;
    }

    std::vector<std::vector<Cell>> Game::getCellsFromImage(const std::string& data, int height, int width){
        std::vector<std::vector<Cell>> 
            cells(height, std::vector<Cell>(width));
        if(data.empty()) return cells;
        if(data.size() != height * width * sizeof(Cell)) throw std::runtime_error("Invalid image data size");
        for(int i = 0; i < height; i++){
            std::memcpy(cells[i].data(), data.data() + i * width * sizeof(Cell), width * sizeof(Cell));
        }
        return cells;
    }

    GameState Game::calcGameState(){
        bool hasRevealed = false;
        bool hasUnrevealedNonMine = false;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(cells[i][j].isRevealed){
                    hasRevealed = true;
                    if(cells[i][j].isMine) return LOST;
                }
                else if(!cells[i][j].isMine){
                    hasUnrevealedNonMine = true;
                }
            }
        }
        if(!hasRevealed) return STARTING;
        if(!hasUnrevealedNonMine) return WON;
        return RUNNING;
    }

    int Game::countFlagsLeft(){
        int count = 0;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(!cells[i][j].isFlagged && cells[i][j].isMine) count++;
            }
        }
        return count;
    }

    bool Game::sweep(int x, int y){
        if(state == STARTING){ 
            startGame(x, y);
            return true;
        }
        if(state != RUNNING) return false;
        if(cells[y][x].isFlagged) return false;
        if(cells[y][x].isRevealed){ 
            if(!isFastSweep) return false;
            if(countAdjacentFlags(x, y) != countAdjacentMines(x, y)) return false;
            sweepAdjacent(x, y);
        }
        else{
            cells[y][x].isRevealed = true;
            if(countAdjacentMines(x, y) == 0) sweepAdjacent(x, y);
        }
        state = calcGameState();
        return true;
    }

    int Game::countAdjacentMines(int x, int y){
        int count = 0;
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(x + j < 0 || x + j >= width || y + i < 0 || y + i >= height || !(j || i)) continue;
                if(cells[y + i][x + j].isMine) count++;
            }
        }
        return count;
    }

    int Game::countAdjacentFlags(int x, int y){
        int count = 0;
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(x + j < 0 || x + j >= width || y + i < 0 || y + i >= height || !(j || i)) continue;
                if(cells[y + i][x + j].isFlagged) count++;
            }
        }
        return count;
    }

    void Game::sweepAdjacent(int x, int y){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(x + j < 0 || x + j >= width || y + i < 0 || y + i >= height || !(j || i)) continue;
                if(cells[y + i][x + j].isRevealed || cells[y + i][x + j].isFlagged) continue;
                sweep(x + j, y + i);
            }
        }
    }

    bool Game::toggleFlag(int x, int y){
        if(state != RUNNING) return false;
        if(cells[y][x].isRevealed){
            if(!isFastFlag) return false;
            if(countAdjacentUnRevealed(x, y) == countAdjacentMines(x, y)) flagAdjacent(x, y);
            else return false;
        }else{
            cells[y][x].isFlagged = !cells[y][x].isFlagged;
        }
        flagsLeft = countFlagsLeft();
        return true;
    }
    bool Game::setFlag(int x, int y, bool flag){
        if(state != RUNNING) return false;
        if(cells[y][x].isRevealed){
            if(!isFastFlag) return false;
            if(countAdjacentUnRevealed(x, y) == countAdjacentMines(x, y)) flagAdjacent(x, y);
            else return false;
        }else{
            cells[y][x].isFlagged = flag;
        }
        flagsLeft = countFlagsLeft();
        return true;
    }
    int Game::countAdjacentUnRevealed(int x, int y){
        int count = 0;
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(x + j < 0 || x + j >= width || y + i < 0 || y + i >= height || !(j || i)) continue;
                if(!cells[y + i][x + j].isRevealed) count++;
            }
        }
        return count;
    }
    int Game::countAdjacentRevealed(int x, int y){
        int count = 0;
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(x + j < 0 || x + j >= width || y + i < 0 || y + i >= height || !(j || i)) continue;
                if(cells[y + i][x + j].isRevealed) count++;
            }
        }
        return count;
    }
    void Game::flagAdjacent(int x, int y){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(x + j < 0 || x + j >= width || y + i < 0 || y + i >= height || !(j || i)) continue;
                if(cells[y + i][x + j].isRevealed) continue;
                cells[y + i][x + j].isFlagged = true;
            }
        }
    }
    Picture Game::getPicture(){
        Picture picture;
        picture.width = width*4+1;
        picture.height = height*2+1;
        picture.data.resize(picture.height, std::vector<Pixel>(picture.width));


        for(int i = 0; i < picture.height; i += 2){
            for(int j = 0; j < picture.width; j++){
                if(j % 4 == 0)
                    picture.data[i][j] = {"┼", DEFAULT_FG, DEFAULT_BG};
                else
                    picture.data[i][j] = {"─", DEFAULT_FG, DEFAULT_BG};
            }
        }
        for(int i = 0; i < picture.height; i++){
            if(i % 2 == 0) continue;
            for(int j = 0; j < picture.width; j += 4){
                picture.data[i][j] = {"│", DEFAULT_FG, DEFAULT_BG};
            }
        }
        for(int i = 0; i < picture.width; i+= 4){
            picture.data[0][i] = {"┬", DEFAULT_FG, DEFAULT_BG};
            picture.data[picture.height-1][i] = {"┴", DEFAULT_FG, DEFAULT_BG};
        }
        for(int i = 0; i < picture.height; i+= 2){
            picture.data[i][0] = {"├", DEFAULT_FG, DEFAULT_BG};
            picture.data[i][picture.width-1] = {"┤", DEFAULT_FG,DEFAULT_BG};
        }

        picture.data[0][0] = {"┌", DEFAULT_FG, DEFAULT_BG};
        picture.data[0][picture.width-1] = {"┐", DEFAULT_FG, DEFAULT_BG};
        picture.data[picture.height-1][0] = {"└", DEFAULT_FG, DEFAULT_BG};
        picture.data[picture.height-1][picture.width-1 ] = {"┘", DEFAULT_FG, DEFAULT_BG};

        Color numberColors[] = {DEFAULT_FG, BRIGHT_BLUE_FG, BRIGHT_GREEN_FG, BRIGHT_RED_FG, BRIGHT_MAGENTA_FG, BRIGHT_CYAN_FG, BRIGHT_YELLOW_FG, BRIGHT_WHITE_FG};

        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                Pixel pixel;
                if(cells[i][j].isMine && state == LOST) pixel = {"󰷚", RED_FG, DEFAULT_BG}; 
                else if(cells[i][j].isFlagged) pixel = {"󰈿", BRIGHT_RED_FG, DEFAULT_BG};
                else if(cells[i][j].isRevealed){
                    if(cells[i][j].isMine) pixel = {"󰷚", RED_FG, DEFAULT_BG};
                    else if(countAdjacentMines(j, i) == 0) pixel = {" ", DEFAULT_FG, DEFAULT_BG};
                    else pixel = {std::to_string(countAdjacentMines(j, i)), numberColors[countAdjacentMines(j, i)], DEFAULT_BG};
                }
                else pixel = {" ", DEFAULT_FG, BRIGHT_BLACK_BG};

                if(cursorX == j && cursorY == i){
                    pixel.bgColor = YELLOW_BG;
                }

                picture.data[i*2+1][j*4+2] = pixel;
            }
        }
        
        return picture;
    }
}
