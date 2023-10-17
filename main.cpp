#include <iostream>
#include "ra2ob.hpp"

int main() {

    Ra2ob& g = Ra2ob::getInstance();

    g.startLoop(false);

    while (true) {
        Sleep(1000);

        //std::cout << g._view.viewToString();
        std::cout << g._view.viewToJson().dump() << std::endl;
    }

    return 0;
}