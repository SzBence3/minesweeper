#pragma once 

#include "Game.h"

namespace tms{
    Game::Image getGameImage(
            std::vector<std::string> args,
            std::string configFile
        );
}
