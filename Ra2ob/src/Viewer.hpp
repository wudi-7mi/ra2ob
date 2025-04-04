#ifndef RA2OB_SRC_VIEWER_HPP_
#define RA2OB_SRC_VIEWER_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "./Datatypes.hpp"
#include "./third_party/json.hpp"

using json = nlohmann::json;

namespace Ra2ob {

class Viewer {
public:
    Viewer();
    ~Viewer();

    json exportJson(tagGameInfo gi, int mode = 0);
    void print(tagGameInfo gi, int mode = 0, int indent = 0);
    std::string uint32ToHex(uint32_t num);
    std::array<std::string, MAXPLAYER> vecToHex(const std::array<uint32_t, MAXPLAYER>& source);
};

inline Viewer::Viewer() {}

/**
 * mode 0 - brief, 1 - full, 2 - debug
 */
inline json Viewer::exportJson(tagGameInfo gi, int mode) {
    json j;

    if (mode == 2) {
        j["debug"]["playerBase"]   = vecToHex(gi.debug.playerBase);
        j["debug"]["buildingBase"] = vecToHex(gi.debug.buildingBase);
        j["debug"]["infantryBase"] = vecToHex(gi.debug.infantryBase);
        j["debug"]["tankBase"]     = vecToHex(gi.debug.tankBase);
        j["debug"]["aircraftBase"] = vecToHex(gi.debug.aircraftBase);
        j["debug"]["houseType"]    = vecToHex(gi.debug.houseType);

        return j;
    }

    if (!(GOODINTENTION || gi.isObserver)) {
        j["status"] = "Not Observer";
        return j;
    }

    if (gi.currentFrame < 5) {
        j["status"] = "Preparing";
        return j;
    }

    if (gi.isGameOver) {
        j["status"] = "Gameover";
        return j;
    }

    j["status"] = "Running";

    j["game"]["version"]      = gi.gameVersion == "Yr" ? "Yr" : "Ra2";
    j["game"]["mapName"]      = gi.mapName;
    j["game"]["currentFrame"] = gi.currentFrame;

    for (auto& p : gi.players) {
        json jp;

        if (mode == 0 && !p.valid) {
            continue;
        }

        jp["panel"]["playerName"]  = p.panel.playerNameUtf;
        jp["panel"]["balance"]     = p.panel.balance;
        jp["panel"]["creditSpent"] = p.panel.creditSpent;
        jp["panel"]["powerDrain"]  = p.panel.powerDrain;
        jp["panel"]["powerOutput"] = p.panel.powerOutput;
        jp["panel"]["color"]       = "#" + p.panel.color;
        jp["panel"]["country"]     = p.panel.country;

        jp["status"]["infantrySelfHeal"] = p.status.infantrySelfHeal;
        jp["status"]["unitSelfHeal"]     = p.status.unitSelfHeal;

        for (auto& u : p.units.units) {
            json ju;

            if (mode == 0 && u.num == 0) {
                continue;
            }

            if (u.show == false) {
                continue;
            }

            ju["unitName"] = u.unitName;
            ju["num"]      = u.num;

            if (mode == 1) {
                ju["index"] = u.index;
            }

            jp["units"].push_back(ju);
        }

        if (!p.building.list.empty()) {
            json jb;

            for (auto& b : p.building.list) {
                json bl;

                bl["name"]     = b.name;
                bl["progress"] = b.progress;
                bl["number"]   = b.number;
                if (b.progress == 54) {
                    bl["status"] = "Ready";
                } else if (b.status == 1) {
                    bl["status"] = "On Hold";
                } else {
                    bl["status"] = "Building";
                }
                jb["producingList"].push_back(bl);
            }
            jp["producingList"] = jb;
        }

        j["players"].push_back(jp);
    }

    return j;
}

/**
 * mode 0 - brief, 1 - full, 2 - debug
 */
inline void Viewer::print(tagGameInfo gi, int mode, int indent) {
    if (mode == 2) {
        json j;

        std::array<std::string, MAXPLAYER> playerName;
        for (int i = 0; i < MAXPLAYER; i++) {
            playerName[i] = gi.players[i].panel.playerName;
        }

        j["debug"]["playerBase"]   = vecToHex(gi.debug.playerBase);
        j["debug"]["buildingBase"] = vecToHex(gi.debug.buildingBase);
        j["debug"]["infantryBase"] = vecToHex(gi.debug.infantryBase);
        j["debug"]["tankBase"]     = vecToHex(gi.debug.tankBase);
        j["debug"]["aircraftBase"] = vecToHex(gi.debug.aircraftBase);
        j["debug"]["houseType"]    = vecToHex(gi.debug.houseType);

        j["debug"]["playerTeamNumber"]   = gi.debug.playerTeamNumber;
        j["debug"]["playerDefeatFlag"]   = gi.debug.playerDefeatFlag;
        j["debug"]["playerGameoverFlag"] = gi.debug.playerGameoverFlag;
        j["debug"]["playerWinnerFlag"]   = gi.debug.playerWinnerFlag;

        std::cout << "[pid]          " << gi.debug.setting.pid << std::endl;
        std::cout << "[gamePath]     " << gi.debug.setting.gamePath << std::endl;
        std::cout << "[isReplay]     " << gi.debug.setting.isReplay << std::endl;
        std::cout << "[mapName]      " << gi.debug.setting.mapName << std::endl;
        std::cout << "[screenSize]   " << gi.debug.setting.screenWidth << "*"
                  << gi.debug.setting.screenHeight << std::endl;
        std::cout << "[fullScreen]   " << gi.debug.setting.fullScreen << std::endl;
        std::cout << "[windowed]     " << gi.debug.setting.windowed << std::endl;
        std::cout << "[border]       " << gi.debug.setting.border << std::endl;
        std::cout << "[render]       " << gi.debug.setting.renderer << std::endl;
        std::cout << "[displayMode]  " << gi.debug.setting.display << std::endl;

        std::cout << "[playerName]   ";
        for (int i = 0; i < MAXPLAYER; i++) {
            std::cout << gi.players[i].panel.playerName << " ";
        }
        std::cout << std::endl;
        std::cout << "[playerBase]   " << j["debug"]["playerBase"].dump() << std::endl;
        std::cout << "[buildingBase] " << j["debug"]["buildingBase"].dump() << std::endl;
        std::cout << "[infantryBase] " << j["debug"]["infantryBase"].dump() << std::endl;
        std::cout << "[tankBase]     " << j["debug"]["aircraftBase"].dump() << std::endl;
        std::cout << "[aircraftBase] " << j["debug"]["aircraftBase"].dump() << std::endl;
        std::cout << "[houseType]    " << j["debug"]["houseType"].dump() << std::endl;

        std::cout << "[playerTeamNumber]" << j["debug"]["playerTeamNumber"].dump() << std::endl;
        std::cout << "[playerDefeatFlag]" << j["debug"]["playerDefeatFlag"].dump() << std::endl;
        std::cout << "[playerGameoverFlag]" << j["debug"]["playerGameoverFlag"].dump() << std::endl;
        std::cout << "[playerWinnerFlag]" << j["debug"]["playerWinnerFlag"].dump() << std::endl;

        return;
    }

    if (!(GOODINTENTION || gi.isObserver)) {
        std::cout << "This player is not observer.";
        return;
    }

    if (gi.currentFrame < 5) {
        std::cout << "Game preparing.";
        return;
    }

    if (gi.isGameOver) {
        std::cout << "Game Over.";
        return;
    }

    std::cout << "Game Version: ";
    if (gi.gameVersion == "Yr") {
        std::cout << "Yr | ";
    } else {
        std::cout << "Ra2 | ";
    }

    std::cout << "Map Name: " << gi.mapName << " | ";

    std::cout << "Game Time: " << convertFrameToTimeString(gi.currentFrame, GAMESPEED) << " | ";

    if (gi.isGamePaused) {
        std::cout << "[Paused]"
                  << " ";
    }
    std::cout << "\n";

    std::vector<int> teamList;  // 0: no team

    for (auto& p : gi.players) {
        if (mode == 0 && !p.valid) {
            continue;
        }

        int teamIndex;
        auto it = std::find(teamList.begin(), teamList.end(), p.status.teamNumber);
        if (it != teamList.end()) {
            teamIndex = std::distance(teamList.begin(), it);
        } else {
            teamList.push_back(p.status.teamNumber);
            teamIndex = teamList.size() - 1;
        }
        std::cout << "Team" << teamIndex << " ";

        int cval = std::stoi(p.panel.color, 0, 16);

        std::cout << p.panel.playerName;

        if (COLORMAP.find(cval) == COLORMAP.end()) {
            std::cout << " ";
        } else {
            std::cout << " " << COLORMAP.at(cval) << "  " << STYLE_OFF;
        }

        std::cout << " " << p.panel.country;
        std::cout << " Balance: " << p.panel.balance;
        std::cout << " Power: " << p.panel.powerDrain << "/" << p.panel.powerOutput;
        std::cout << " Credit: " << p.panel.creditSpent;

        if (p.status.infantrySelfHeal || p.status.unitSelfHeal) {
            std::cout << " Auto Repair: ";
        }
        if (p.status.infantrySelfHeal) {
            std::cout << "[Infantry+] ";
        }
        if (p.status.unitSelfHeal) {
            std::cout << "[Tank+] ";
        }

        std::cout << " Kills/Lost/Built/Alive: " << p.score.kills << "/" << p.score.lost << "/"
                  << p.score.built << "/" << p.score.alive;

        std::cout << "\n";

        if (!p.superTimer.list.empty()) {
            std::cout << "Super Weapons: ";
            for (auto& s : p.superTimer.list) {
                if (s.status == 1) {
                    std::cout << s.name << ": On Hold | ";
                    continue;
                }
                std::cout << s.name << ": " << converFrameToGameTimeString(s.left) << " | ";
            }
            std::cout << "\n";
        }

        for (auto& u : p.units.units) {
            if (mode == 0 && u.num == 0) {
                continue;
            }

            if (u.num < 0) {
                continue;
            }

            if (u.show == false) {
                continue;
            }

            std::cout << u.unitName << ": " << u.num;

            if (mode == 1) {
                std::cout << " index=" << u.index;
            }

            std::cout << "\n";
        }

        if (!p.building.list.empty()) {
            std::cout << "Producing List: "
                      << "\n";

            for (auto& b : p.building.list) {
                std::cout << b.name << " " << b.progress << "/54 ";
                if (b.progress == 54) {
                    std::cout << "Ready ";
                } else if (b.status == 1) {
                    std::cout << "On Hold ";
                } else {
                    std::cout << "Building ";
                }

                if (b.number > 1) {
                    std::cout << "[" << b.number << "]";
                }

                std::cout << "\n";
            }
        }

        for (int i = 0; i < RULER_MULT; i++) {
            std::cout << STR_RULER;
        }

        std::cout << std::endl;
    }
}

inline std::string Viewer::uint32ToHex(uint32_t num) {
    std::stringstream ss;
    ss << std::hex << num;
    return ss.str();
}

inline std::array<std::string, MAXPLAYER> Viewer::vecToHex(
    const std::array<uint32_t, MAXPLAYER>& source) {
    std::array<std::string, MAXPLAYER> ret{};

    for (int i = 0; i < source.size(); i++) {
        ret[i] = uint32ToHex(source[i]);
    }

    return ret;
}

inline Viewer::~Viewer() {}

}  // namespace Ra2ob

#endif  // RA2OB_SRC_VIEWER_HPP_
