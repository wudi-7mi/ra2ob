#ifndef RA2OB_SRC_VIEWER_HPP_
#define RA2OB_SRC_VIEWER_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "./Constants.hpp"
#include "./Datatypes.hpp"
#include "./json.hpp"

using json = nlohmann::json;

namespace Ra2ob {

class Viewer {
public:
    Viewer();
    ~Viewer();

    json exportJson(tagGameInfo gi, int mode = 0);
    void print(tagGameInfo gi, int mode = 0, int indent = 0);
    std::string uint32ToHex(uint32_t num);
    std::vector<std::string> vecToHex(std::vector<uint32_t> source);
};

Viewer::Viewer() {}

/**
 * mode 0 - brief, 1 - full, 2 - debug
 */
json Viewer::exportJson(tagGameInfo gi, int mode) {
    json j;

    if (mode > 0) {
        j["debug"]["playerBase"]   = vecToHex(gi.debug.playerBase);
        j["debug"]["buildingBase"] = vecToHex(gi.debug.buildingBase);
        j["debug"]["infantryBase"] = vecToHex(gi.debug.infantryBase);
        j["debug"]["tankBase"]     = vecToHex(gi.debug.tankBase);
        j["debug"]["aircraftBase"] = vecToHex(gi.debug.aircraftBase);
        j["debug"]["houseType"]    = vecToHex(gi.debug.houseType);
    }

    if (mode == 2) {
        return j;
    }

    for (auto& p : gi.players) {
        json jp;

        if (mode == 0 && !p.valid) {
            continue;
        }

        jp["panel"]["playerName"]  = p.panel.playerName;
        jp["panel"]["balance"]     = p.panel.balance;
        jp["panel"]["creditSpent"] = p.panel.creditSpent;
        jp["panel"]["powerDrain"]  = p.panel.powerDrain;
        jp["panel"]["powerOutput"] = p.panel.powerOutput;
        jp["panel"]["color"]       = p.panel.color;
        jp["panel"]["country"]     = p.panel.country;

        for (auto& u : p.units.units) {
            json ju;

            if (mode == 0 && u.num == 0) {
                continue;
            }

            ju["unitName"] = u.unitName;
            ju["num"]      = u.num;

            if (mode == 1) {
                ju["index"] = u.index;
            }

            jp["units"].push_back(ju);
        }

        // Todo: Add buildings.

        j["players"].push_back(jp);
    }

    return j;
}

/**
 * mode 0 - brief, 1 - full, 2 - debug
 */
void Viewer::print(tagGameInfo gi, int mode, int indent) {
    json j = exportJson(gi, mode);

    if (indent == 0) {
        std::cout << j.dump() << std::endl;
    } else {
        std::cout << j.dump(indent) << std::endl;
    }
}

std::string Viewer::uint32ToHex(uint32_t num) {
    std::stringstream ss;
    ss << std::hex << num;
    return ss.str();
}

std::vector<std::string> Viewer::vecToHex(std::vector<uint32_t> source) {
    std::vector<std::string> ret;

    for (auto& it : source) {
        ret.push_back(uint32ToHex(it));
    }

    return ret;
}

Viewer::~Viewer() {}

}  // namespace Ra2ob

#endif  // RA2OB_SRC_VIEWER_HPP_
