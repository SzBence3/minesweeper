#include "Game.h"
#include <algorithm>
#include <random>

#define MAX_ITERATIONS 500

namespace tms{
    void Game::generateMines(int x, int y){
        std::vector<std::pair<int, int>> toPlace;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(!(j == x && i == y) && (std::abs(j - x) > 1 || std::abs(i - y) > 1)){
                    toPlace.emplace_back(j, i);
                }
            }
        }
        std::shuffle(toPlace.begin(), toPlace.end(), std::mt19937(std::random_device()()));

        for(int i = 0; i < toPlace.size() && i < flagsLeft; i++){
            cells[toPlace[i].second][toPlace[i].first].isMine = true;
        }
        state = RUNNING;
        sweep(x, y);
    }
    void Game::startGame(int x, int y){
        int iteration = 0;
        generateMines(x, y);
        
       while(!isSolvable()){
            iteration++;
            if(iteration > MAX_ITERATIONS){
                std::cerr << "Too many iterations, giving up" << std::endl;
                throw std::runtime_error("Failed to generate solvable game");
                return;
            }
            reset();
            std::cerr << "Generated unsolvable game, regenerating... (iteration " << iteration << ")" << std::endl;
            generateMines(x, y);

       }
       std::cerr << "Generated solvable game in " << iteration << " iterations" << std::endl;
    }
    bool Game::isSolvable(){
        Game copy = *this;
        copy.solve();
        return copy.getState() == WON;
    }
    void Game::reset(){
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                cells[i][j].isMine = false;
                cells[i][j].isRevealed = false;
                cells[i][j].isFlagged = false;
            }
        }
    }
}
