#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Colors.h"

namespace tms
{
    struct Pixel{
        std::string ch = " ";
        char fgColor = DEFAULT_FG;
        char bgColor = DEFAULT_BG;
    };

    struct Picture{
        int width;
        int height;
        std::vector<std::vector<Pixel>> data;
    };

    class Screen{
        std::vector<Picture> pictures;
    public:
        Screen() = default;
        ~Screen() = default;
        void clear(){
                std::cout << "\033[2J\033[H" << std::flush;
        }
        void drawPicture(Picture pic, int x = 0, int y = 0);
        char getInput();
    };
}
