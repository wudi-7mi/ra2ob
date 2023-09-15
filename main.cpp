#include <iostream>
#include "ra2ob.hpp"

int main() {
    while (true) {
        Game g = Game();

        g.initDatas();
        if (g.getHandle() == 0) {
            g.initAddrs();

            std::cout << "Player numbers: " << g.hasPlayer() << std::endl;

            while (true) {
                g.initAddrs();
                std::cout << "Player numbers: " << g.hasPlayer() << std::endl;
                g.showInfo();
                Sleep(500);
                system("cls");

                if (g.getHandle() != 0) {
                    break;
                }
            }
        }
        Sleep(1000);
    }

    return 0;
}