#include "Screen.h"
#include <iostream>

#include <unistd.h>
#include <termios.h>

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON; // Disable canonical mode (line buffering)
    old.c_lflag &= ~ECHO;   // Disable echoing of characters
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}


namespace tms{
    void Screen::drawPicture(Picture pic, int x, int y){
        x++; y++;
        for(int i = 0; i < pic.height; i++){
            // Move cursor to the correct position
            std::cout << "\033[" << (y + i) << ";" << x << "H";
            for(int j = 0; j < pic.width; j++){
                Pixel& pixel = pic.data[i][j];
                std::cout << "\033[" << (int)pixel.fgColor << ";" << (int)pixel.bgColor << "m" << pixel.ch;
            }
            std::cout << "\033[0m" << std::flush;
        }
    }
    char Screen::getInput(){
        return getch();
    }
}
