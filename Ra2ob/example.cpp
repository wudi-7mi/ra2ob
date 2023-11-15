#include "Ra2ob"

int main() {
    Ra2ob::Game& g = Ra2ob::Game::getInstance();

    g.startLoop();

    while (true) {
        Sleep(1000);
    }

    return 0;
}
