#include "Game.h"
#include <algorithm>
#include <random>

namespace tms{
    void Game::startGame(int x, int y){
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
    }
}
