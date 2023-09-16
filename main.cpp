#include <iostream>
#include "ra2ob.hpp"

int main() {
    while (true) {
        Ra2ob g = Ra2ob();

        g.initDatas();

        if (g.getHandle() == 0) {
            if (g.initAddrs()) {
                while (true) {
                    if (!g.showInfo()) {
                        system("cls");
                        break;
                    }

                    if (!g.initAddrs()) {
                        system("cls");
                        break;
                    }

                    std::cout << "Player numbers: " << g.hasPlayer() << std::endl;

                    Sleep(500);
                    system("cls");
                }
            }

        }
        Sleep(1000);
    }

    return 0;
}