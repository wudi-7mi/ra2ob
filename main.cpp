/*
    Copyright (C) 2023  wudi-7mi  wudi7mi@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>

#include "./ra2ob.hpp"

int main() {
    Ra2ob& g = Ra2ob::getInstance();

    g.startLoop(false);

    while (true) {
        Sleep(1000);

        // g._view.viewPrint();
        // std::cout << g._view.viewToJson().dump() << std::endl;
        // std::cout << g._view.viewToJsonFull().dump() << std::endl;
        std::cout << g._view.getPlayerPanelInfo(0).dump() << std::endl;
        std::cout << g._view.getPlayerUnitInfo(0).dump() << std::endl;
    }

    return 0;
}
