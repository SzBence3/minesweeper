#include "Config.h"
#include <fstream>
#include <map>

namespace tms{
    Game::Image getGameImage(
            std::vector<std::string> args,
            std::string configFile
        )
    {
        Game::Image image = {10, 10, true, true, 20};
        // Default values
        std::map<std::string, Game::Image> Modes = {
            {"easy", {9, 9, true, true, 15}},
            {"medium", {16, 16, true, true, 40}},
            {"hard", {16, 32, true, true, 99}}
        };
        // Load from config file
        std::ifstream infile(configFile);

        if (infile.is_open()) {
            std::string line;
            while (std::getline(infile, line)) {
                auto delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = line.substr(0, delimiterPos);
                    std::string value = line.substr(delimiterPos + 1);
                    if (key == "width") image.width = std::stoi(value);
                    else if (key == "height") image.height = std::stoi(value);
                    else if (key == "minecount") image.minecount = std::stoi(value);
                    else if (key == "isFastFlag") image.isFastFlag = (value == "true");
                    else if (key == "isFastSweep") image.isFastSweep = (value == "true");
                }
            }
            infile.close();
        }


        // Override with command-line arguments
        for (const auto& arg : args) {
            auto delimiterPos = arg.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = arg.substr(0, delimiterPos);
                std::string value = arg.substr(delimiterPos + 1);
                if (key == "--width") image.width = std::stoi(value);
                else if (key == "--height") image.height = std::stoi(value);
                else if (key == "--minecount") image.minecount = std::stoi(value);
                else if (key == "--isFastFlag") image.isFastFlag = (value == "true");
                else if (key == "--isFastSweep") image.isFastSweep = (value == "true");
                else if (key == "--mode") {
                    auto modeIt = Modes.find(value);
                    if (modeIt != Modes.end()) {
                        image = modeIt->second;
                    }
                }
            }
        }

        return image;
    }
}
