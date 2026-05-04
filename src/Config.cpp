#include "Config.h"
#include <fstream>
#include <map>

namespace tms{
    Game::Image getGameImage(
            std::vector<std::string> args,
            std::string configFile
        )
    {
        Game::Image image = {10, 10, 30, "", Game::Settings()};
        // Default values
        std::map<std::string, Game::Image> Modes = {
            {"easy", {9, 9, 20, "", Game::Settings()}},
            {"medium", {16, 16, 70, "", Game::Settings()}},
            {"hard", {16, 32, 125, "", Game::Settings()}}
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
                    else if (key == "isFastFlag") image.settings.isFastFlag = (value == "true");
                    else if (key == "isFastSweep") image.settings.isFastSweep = (value == "true");
                }
            }
            infile.close();
        }


        // Override with command-line arguments
        if (args.size() == 1 && args[0] == "--help") {
            std::cout << "Usage: tms [options]\n"
                      << "Options:\n"
                      << "  --width=<value>       Set the width of the game board\n"
                      << "  --height=<value>      Set the height of the game board\n"
                      << "  --minecount=<value>  Set the number of mines\n"
                      << "  --isFastFlag=<true|false>  Enable or disable fast flagging\n"
                      << "  --isFastSweep=<true|false> Enable or disable fast sweeping\n"
                      << "  --mode=<easy|medium|hard>  Set predefined game mode\n"
                      << "  --help                Show this help message\n"
                      << "You can move with hjkl and place flags with f and sweep with s\n"
                      << std::endl;
            exit(0);
        }
        for (const auto& arg : args) {
            auto delimiterPos = arg.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = arg.substr(0, delimiterPos);
                std::string value = arg.substr(delimiterPos + 1);
                if (key == "--width") image.width = std::stoi(value);
                else if (key == "--height") image.height = std::stoi(value);
                else if (key == "--minecount") image.minecount = std::stoi(value);
                else if (key == "--isFastFlag") image.settings.isFastFlag = (value == "true");
                else if (key == "--isFastSweep") image.settings.isFastSweep = (value == "true");
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
