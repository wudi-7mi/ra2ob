#include <iostream>
#include "ra2ob.hpp"

int main() {
    while (true) {
        Ra2ob& g = Ra2ob::getInstance();

        g.initDatas();

        g.startLoop();

        Sleep(1000);
    }

    return 0;
}