#include <iostream>
#include "ra2ob.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

int main() {

    Ra2ob& g = Ra2ob::getInstance();

    g.startLoop(false);

    while (true) {
        Sleep(1000);

        std::cout << g._view.viewToString();
    }

    return 0;
}