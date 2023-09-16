#include <iostream>
#include "ra2ob.hpp"

int main() {
    while (true) {
        Ra2ob g = Ra2ob();

        g.initDatas();

        if (g.getHandle() == 0) {
            if (g.initAddrs()) {
                while (true) {
                    std::cout << "Player numbers: " << g.hasPlayer() << std::endl;

                    if (!g.refreshInfo()) {
                        system("cls");
                        break;
                    }

                    g.exportInfo();

                    std::cout << std::endl;

                    if (!g.initAddrs()) {
                        system("cls");
                        break;
                    }


                    Sleep(500);
                    system("cls");
                }
            }

        }
        Sleep(1000);
    }

    return 0;
}