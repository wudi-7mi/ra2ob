#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include <fstream>
#include <memory>
#include <Windows.h>
#include <TlHelp32.h>   //for PROCESSENTRY32, needs to be included after windows.h
#include <locale>
#include <codecvt> 
#include <map>

#define MAXPLAYER 8
#define INVALIDCLASS 0xffffffffu

#define BOOLSIZE 1
#define NUMSIZE 4
#define PTRSIZE 4
#define STRNAMESIZE 32
#define STRCOUNTRYSIZE 25

#define FIXEDOFFSET 0xa8b230
#define CLASSBASEARRAYOFFSET 0xa8022c
#define PLAYERBASEARRAYPTROFFSET 0x1180
#define HOUSETYPEOFFSET 0x34
#define BUILDINGOFFSET 0x5554
#define TANKOFFSET 0x5568
#define INFANTRYOFFSET 0x557c
#define AIRCRAFTOFFSET 0x5590

#define STRNAMEOFFSET 0x1602a
#define STRCOUNTRYOFFSET 0x24

#define UNITSAFE 4096

using json = nlohmann::json;

class Game {

public:
    Game() {

    }

    class DataBase {

    public:
        DataBase(std::string name, uint32_t offset) {
            m_name = name;
            m_offset = offset;
            m_value = std::vector<uint32_t>(MAXPLAYER, 0);
            m_size = NUMSIZE;
        }

        void showInfo() {
            std::cout << "Data name: " << m_name << std::endl;
            std::cout << "Offset: 0x" << std::hex << m_offset << std::endl;
            std::cout << "Size: " << m_size << std::endl;
        }

        std::string getName() {
            return m_name;
        }

        uint32_t getValueByIndex(int index) {
            if (index >= MAXPLAYER) {
                std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
                return -1;
            }
            return m_value[index];
        }

        void setValueByIndex(int index, uint32_t value) {
            if (index >= MAXPLAYER) {
                std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
            }
            m_value[index] = value;
        }

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets) {
            for (int i = 0; i < baseOffsets.size(); i++) {
                if (baseOffsets[i] == 0) {
                    continue;
                }


                uint32_t buf;

                ReadProcessMemory(
                    pHandle,
                    reinterpret_cast<const void*>(baseOffsets[i] + m_offset),
                    &buf,
                    m_size,
                    nullptr
                );
                m_value[i] = buf;
            }
        }

    protected:
        std::string m_name;
        std::vector<uint32_t> m_value;
        uint32_t m_offset;
        uint32_t m_size;
    };


    class Numeric : public DataBase {

    public:
        Numeric(std::string name, uint32_t offset)
            : DataBase(name, offset) {}

    };


    class Unit : public DataBase {

    public:
        enum factionType { Soviet, Allied, UnknownFaction };
        enum unitType { Building, Tank, Infantry, Aircraft, UnknownUnit };

        Unit(
            std::string name,
            uint32_t offset,
            Unit::factionType ft,
            Unit::unitType ut
        ) : DataBase(name, offset) {
            m_factionType = ft,
                m_unitType = ut;
        }

        factionType getFactionType() {
            return m_factionType;
        }

        unitType getUnitType() {
            return m_unitType;
        }

    protected:
        factionType m_factionType;
        unitType m_unitType;

    };


    class StrName : public DataBase {

    public:
        StrName(std::string name, uint32_t offset)
            : DataBase(name, offset) {
            m_value = std::vector<std::string>(MAXPLAYER, "");
            m_size = STRNAMESIZE;
        }

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets) {
            for (int i = 0; i < baseOffsets.size(); i++) {
                if (baseOffsets[i] == 0) {
                    continue;
                }

                wchar_t buf[STRNAMESIZE];

                ReadProcessMemory(
                    pHandle,
                    reinterpret_cast<const void*>(baseOffsets[i] + m_offset),
                    &buf,
                    m_size,
                    nullptr
                );
                std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
                m_value[i] = convert.to_bytes(buf);
            }
        }

        std::string getValueByIndex(int index) {
            if (index >= MAXPLAYER) {
                std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
                return "";
            }
            return m_value[index];
        }

        void setValueByIndex(int index, std::string value) {
            if (index >= MAXPLAYER) {
                std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
            }
            m_value[index] = value;
        }

    protected:
        std::vector<std::string> m_value;

    };


    class StrCountry : public StrName {

    public:
        StrCountry(std::string name, uint32_t offset)
            : StrName(name, offset) {
            m_size = STRCOUNTRYSIZE;
        }

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets) {
            for (int i = 0; i < baseOffsets.size(); i++) {
                if (baseOffsets[i] == 0) {
                    continue;
                }

                char buf[STRCOUNTRYSIZE];

                ReadProcessMemory(
                    pHandle,
                    reinterpret_cast<const void*>(baseOffsets[i] + m_offset),
                    &buf,
                    m_size,
                    nullptr
                );
                std::string res = buf;
                m_value[i] = m_countryMap[res];
            }
        }

        std::map<std::string, std::string> m_countryMap = {
            {"Americans", "Americans"},
            {"Alliance", "Korea"},
            {"French", "French"},
            {"Germans", "Germans"},
            {"British", "British"},
            {"Africans", "Libya"},
            {"Arabs", "Iraq"},
            {"Russians", "Russians"}
        };
    };


    class WinOrLose : public DataBase {

    public:
        WinOrLose(std::string name, uint32_t offset)
            : DataBase(name, offset) {
            m_value = std::vector<bool>(MAXPLAYER, false);
            m_size = BOOLSIZE;
        }

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets) {
            for (int i = 0; i < baseOffsets.size(); i++) {
                if (baseOffsets[i] == 0) {
                    continue;
                }

                bool buf;

                ReadProcessMemory(
                    pHandle,
                    reinterpret_cast<const void*>(baseOffsets[i] + m_offset),
                    &buf,
                    m_size,
                    nullptr
                );
                m_value[i] = buf;
            }
        }

        bool getValueByIndex(int index) {
            if (index >= MAXPLAYER) {
                std::cerr << "Error: Index cannot be bigger than MAXPLAYER." << std::endl;
                return false;
            }
            return m_value[index];
        }

        void setValueByIndex(int index, bool value) {
            if (index >= MAXPLAYER) {
                std::cerr << "Error: Index cannot be bigger than MAXPLAYER." << std::endl;
            }
            m_value[index] = value;
        }

    protected:
        std::vector<bool> m_value;

    };

    using Numerics = std::vector<Numeric>;
    using Units = std::vector<Unit>;

    Numerics loadNumericsFromJson(std::string filePath = "../numeric_offsets.json") {
        Numerics numerics;

        std::ifstream f(filePath);
        json data = json::parse(f);

        for (auto& it : data) {
            std::string offset = it["Offset"];
            uint32_t s_offset = std::stoul(offset, nullptr, 16);
            Numeric n(it["Name"], s_offset);
            numerics.push_back(n);
        }

        // for (auto& n : numerics) {
        //   n.showInfo();
        // }

        return numerics;
    }

    Units loadUnitsFromJson(std::string filePath = "../unit_offsets.json") {
        Units units;

        std::ifstream f(filePath);
        json data = json::parse(f);

        for (auto& ft : data.items()) {

            Unit::factionType s_ft;

            if (ft.key() == "Soviet") {
                s_ft = Unit::Soviet;
            }
            else if (ft.key() == "Allied") {
                s_ft = Unit::Allied;
            }
            else {
                s_ft = Unit::UnknownFaction;
            }

            for (auto& ut : data[ft.key()].items()) {

                Unit::unitType s_ut;

                if (ut.key() == "Building") {
                    s_ut = Unit::Building;
                }
                else if (ut.key() == "Tank") {
                    s_ut = Unit::Tank;
                }
                else if (ut.key() == "Infantry") {
                    s_ut = Unit::Infantry;
                }
                else if (ut.key() == "Aircraft") {
                    s_ut = Unit::Aircraft;
                }
                else {
                    s_ut = Unit::UnknownUnit;
                }

                for (auto& u : data[ft.key()][ut.key()]) {

                    if (u.empty()) {
                        continue;
                    }

                    std::string offset = u["Offset"];
                    uint32_t s_offset = std::stoul(offset, nullptr, 16);
                    Unit ub(u["Name"], s_offset, s_ft, s_ut);
                    units.push_back(ub);
                }
            }
        }

        // for (auto& u : units) {
        //   u.showInfo();
        // }

        return units;
    }

    std::vector<std::string> loadViewsFromJson(std::string filePath = "../view.json") {
        std::vector<std::string> ret;

        std::ifstream f(filePath);
        json data = json::parse(f);

        for (auto& v : data) {
            ret.push_back(v);
        }
        return ret;
    }

    void initDatas() {
        _numerics = loadNumericsFromJson();
        _units = loadUnitsFromJson();
        _views = loadViewsFromJson();
    }

    void initAddrs() {
        if (NULL == _pHandle) {
            std::cerr << "No valid process handle, call Game::getHandle() first." << std::endl;
            return;
        }

        uint32_t fixed = getAddr(FIXEDOFFSET);
        uint32_t classBaseArray = getAddr(CLASSBASEARRAYOFFSET);
        uint32_t playerBaseArrayPtr = fixed + PLAYERBASEARRAYPTROFFSET;

        for (int i = 0; i < MAXPLAYER; i++, playerBaseArrayPtr += 4) {
            uint32_t playerBase = getAddr(playerBaseArrayPtr);

            if (playerBase != INVALIDCLASS) {
                uint32_t realPlayerBase = getAddr(playerBase * 4 + classBaseArray);

                _players[i] = true;
                _playerBases[i] = realPlayerBase;
                _buildings[i] = getAddr(realPlayerBase + BUILDINGOFFSET);
                _tanks[i] = getAddr(realPlayerBase + TANKOFFSET);
                _infantrys[i] = getAddr(realPlayerBase + INFANTRYOFFSET);
                _aircrafts[i] = getAddr(realPlayerBase + AIRCRAFTOFFSET);
                _houseTypes[i] = getAddr(realPlayerBase + HOUSETYPEOFFSET);
            }
        }
    }

    int hasPlayer() {
        int count = 0;
        
        for (auto& it : _players) {
            if (it) {
                count++;
            }
        }
        if (count == 0) {
            std::cerr << "No valid player." << std::endl;
        }
        return count;
    }

    void showInfo() {
        if (!hasPlayer()) {
            std::cerr << "No valid player to show info." << std::endl;
            return;
        }

        for (auto& it : _numerics) {
            it.fetchData(_pHandle, _playerBases);
        }

        for (auto& it : _units) {
            if (it.getUnitType() == Unit::Building) {
                it.fetchData(_pHandle, _buildings);
            }
            else if (it.getUnitType() == Unit::Infantry) {
                it.fetchData(_pHandle, _infantrys);
            }
            else if (it.getUnitType() == Unit::Tank) {
                it.fetchData(_pHandle, _tanks);
            }
            else {
                it.fetchData(_pHandle, _aircrafts);
            }
        }

        _strName.fetchData(_pHandle, _playerBases);
        _strCountry.fetchData(_pHandle, _houseTypes);

        for (int i = 0; i < MAXPLAYER; i++) {
            if (!_players[i]) {
                continue;
            }
            std::cout << _strName.getName() << ": " << _strName.getValueByIndex(i) << std::endl;
            std::cout << _strCountry.getName() << ": " << _strCountry.getValueByIndex(i) << std::endl;
            for (auto& it : _numerics) {
                if (std::find(_views.begin(), _views.end(), it.getName()) != _views.end()) {
                    std::cout << it.getName() << ": " << it.getValueByIndex(i) << std::endl;
                }
            }
            for (auto& it : _units) {
                if (std::find(_views.begin(), _views.end(), it.getName()) != _views.end()) {
                    std::cout << it.getName() << ": " << it.getValueByIndex(i) << std::endl;
                }
            }

        }
    }

    void showNames() {
        for (auto& it : _numerics) {
            std::cout << it.getName() << std::endl;
        }
        for (auto& it : _units) {
            std::cout << it.getName() << std::endl;
        }
        std::cout << _strName.getName() << std::endl;
    }

    int getHandle() {
        DWORD pid = 0;
        std::string name = "gamemd-spawn.exe";

        std::unique_ptr<void, decltype(&CloseHandle)> h(
            CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0),
            &CloseHandle
        );

        if (h.get() == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create snapshot" << std::endl;
            return 1;
        }

        PROCESSENTRY32 processInfo {};
        processInfo.dwSize = sizeof(PROCESSENTRY32);

        for (BOOL success = Process32First(h.get(), &processInfo); success; success = Process32Next(h.get(), &processInfo)) {
            if (name == processInfo.szExeFile) {
                pid = processInfo.th32ProcessID;
                std::cout << "PID Found: " << pid << std::endl;
            }
        }

        if (pid == 0) {
            std::cerr << "No Valid PID. Finding \"gamemd-spawn.exe\"." << std::endl;
            return 1;
        }

        HANDLE pHandle = OpenProcess(
            PROCESS_QUERY_INFORMATION |     // Needed to get a process' token
            PROCESS_CREATE_THREAD   |     // For obvious reasons
            PROCESS_VM_OPERATION    |      // Required to perform operations on address space of process (like WriteProcessMemory)
            PROCESS_VM_READ,            // Required for read data
            FALSE,                          // Don't inherit pHandle
            pid
        );

        if (pHandle == NULL)
        {
            std::cerr << "Could not open process\n" << std::endl;
            return 1;
        }

        _pHandle = pHandle;
        return 0;
    }

    uint32_t getAddr(uint32_t offset) {
        uint32_t buf;
        ReadProcessMemory(
            _pHandle,
            reinterpret_cast<const void*>(offset),
            &buf,
            4,
            nullptr
        );
        return buf;
    }

    HANDLE _pHandle;
    Numerics _numerics;
    Units _units;
    StrName _strName = StrName("Player Name", STRNAMEOFFSET);
    StrCountry _strCountry = StrCountry("Country", STRCOUNTRYOFFSET);

    std::vector<bool> _players = std::vector<bool>(MAXPLAYER, false);
    std::vector<std::string> _views;
    std::vector<uint32_t> _playerBases = std::vector<uint32_t>(MAXPLAYER, 0);
    std::vector<uint32_t> _buildings = std::vector<uint32_t>(MAXPLAYER, 0);
    std::vector<uint32_t> _infantrys = std::vector<uint32_t>(MAXPLAYER, 0);
    std::vector<uint32_t> _tanks = std::vector<uint32_t>(MAXPLAYER, 0);
    std::vector<uint32_t> _aircrafts = std::vector<uint32_t>(MAXPLAYER, 0);
    std::vector<uint32_t> _houseTypes = std::vector<uint32_t>(MAXPLAYER, 0);

};


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