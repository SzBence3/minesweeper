#include "utils.h"

namespace tms {
    std::vector<std::string> getArgVector(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(std::string(argv[i]));
        }
        return args;
    }
}
