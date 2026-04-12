#include<iostream>
#include "Screen.h"
#include "Game.h"
#include "Config.h"
#include "GameHandler.h"
#include "utils.h"

int main(int argc, char* argv[])
{

//     tms::Screen screen;
//     tms::Game::Image image;
//     try{
//         image = tms::getGameImage(tms::getArgVector(argc, argv), "~/.config/tms/config.txt");
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }
//     tms::Game game(image);
//
//     tms::playGame(screen, game);
//
    tms::Game::Image image = tms::getGameImage(tms::getArgVector(argc, argv), "~/.config/tms/config.txt");
    tms::Game game(image);
    game.setCursorPosition(2, 2);
    tms::Screen screen;
    
    playGame(screen, game);
    if(game.getState() == tms::WON) std::cout << "You won!" << std::endl;
    else if(game.getState() == tms::LOST) std::cout << "You lost!" << std::endl;

    return 0;
}
