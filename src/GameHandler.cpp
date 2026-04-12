#include "GameHandler.h"


namespace tms{
    void playGame(Screen& screen, Game& game){
        screen.clear();
        auto [h,w] = game.getSize();
        game.setCursorPosition(w/2, h/2);

        while (game.getState() >= 0){
            // screen.clear();
            screen.drawPicture(game.getPicture(), 0,0);
            char input = screen.getInput();
            switch (input){
                case 'h': {
                    auto [x,y] = game.getCursorPosition();
                    game.setCursorPosition(x-1, y);
                    break;
                }
                case 'j': {
                    auto [x,y] = game.getCursorPosition();
                    game.setCursorPosition(x, y+1);
                    break;
                }
                case 'k': {
                    auto [x,y] = game.getCursorPosition();
                    game.setCursorPosition(x, y-1);
                    break;
                }
                case 'l': {
                    auto [x,y] = game.getCursorPosition();
                    game.setCursorPosition(x+1, y);
                    break;
                }
                case 's': {
                    auto [x,y] = game.getCursorPosition();
                    game.sweep(x, y);
                    break;
                }
                case 'f': {
                    auto [x,y] = game.getCursorPosition();
                    game.toggleFlag(x, y);
                    break;
                }
            }
        }
        screen.clear();
        auto picture = game.getPicture();
        screen.drawPicture(picture, 0,0);
    }
}

