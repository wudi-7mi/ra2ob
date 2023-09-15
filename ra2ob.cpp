#include <iostream>
#include <vector>
#include "json.hpp"
#include <fstream>
#include <memory>
#include <Windows.h>
#include <TlHelp32.h>   //for PROCESSENTRY32, needs to be included after windows.h
#include <locale>
#include <codecvt>
#include "ra2ob.hpp"

using json = nlohmann::json;

Game::Game() {}

Game::DataBase::DataBase(std::string name, uint32_t offset) {
    m_name = name;
    m_offset = offset;
    m_value = std::vector<uint32_t>(MAXPLAYER, 0);
    m_size = NUMSIZE;
};

void Game::DataBase::showInfo() {
    std::cout << "Data name: " << m_name << std::endl;
    std::cout << "Offset: 0x" << std::hex << m_offset << std::endl;
    std::cout << "Size: " << m_size << std::endl;
}

std::string Game::DataBase::getName() {
    return m_name;
}

uint32_t Game::DataBase::getValueByIndex(int index) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
        return -1;
    }
    return m_value[index];
}

void Game::DataBase::setValueByIndex(int index, uint32_t value) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
    }
    m_value[index] = value;
}

void Game::DataBase::fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets) {
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

Game::Numeric::Numeric(std::string name, uint32_t offset)
    : DataBase(name, offset) {}

Game::Unit::Unit(
    std::string name,
    uint32_t offset,
    Unit::factionType ft,
    Unit::unitType ut
) : Game::DataBase(name, offset) {
    m_factionType = ft;
    m_unitType = ut;
}

Game::Unit::factionType Game::Unit::getFactionType() {
    return m_factionType;
}

Game::Unit::unitType Game::Unit::getUnitType() {
    return m_unitType;
}

Game::StrName::StrName(std::string name, uint32_t offset)
    : Game::DataBase(name, offset) {
    m_value = std::vector<std::string>(MAXPLAYER, "");
    m_size = STRNAMESIZE;
}

void Game::StrName::fetchData(
    HANDLE pHandle,
    std::vector<uint32_t> baseOffsets
) {
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

std::string Game::StrName::getValueByIndex(int index) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
        return "";
    }
    return m_value[index];
}

void Game::StrName::setValueByIndex(int index, std::string value) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be larger than MAXPLAYER." << std::endl;
    }
    m_value[index] = value;
}

Game::StrCountry::StrCountry(std::string name, uint32_t offset)
    : StrName(name, offset) {
    m_size = STRCOUNTRYSIZE;
}

void Game::StrCountry::fetchData(
    HANDLE pHandle,
    std::vector<uint32_t> baseOffsets
) {
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

Game::WinOrLose::WinOrLose(std::string name, uint32_t offset)
    : Game::DataBase(name, offset) {
    m_value = std::vector<bool>(MAXPLAYER, false);
    m_size = BOOLSIZE;
}

void Game::WinOrLose::fetchData(
    HANDLE pHandle,
    std::vector<uint32_t> baseOffsets
) {
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

bool Game::WinOrLose::getValueByIndex(int index) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be bigger than MAXPLAYER." << std::endl;
        return false;
    }
    return m_value[index];
}

void Game::WinOrLose::setValueByIndex(int index, bool value) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be bigger than MAXPLAYER." << std::endl;
    }
    m_value[index] = value;
}

Game::Numerics Game::loadNumericsFromJson(std::string filePath) {
    Numerics numerics;

    std::ifstream f(filePath);
    json data = json::parse(f);

    for (auto& it : data) {
        std::string offset = it["Offset"];
        uint32_t s_offset = std::stoul(offset, nullptr, 16);
        Numeric n(it["Name"], s_offset);
        numerics.push_back(n);
    }

    return numerics;
}

Game::Units Game::loadUnitsFromJson(std::string filePath) {
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

    return units;
}

std::vector<std::string> Game::loadViewsFromJson(std::string filePath) {
    std::vector<std::string> ret;

    std::ifstream f(filePath);
    json data = json::parse(f);

    for (auto& v : data) {
        ret.push_back(v);
    }

    return ret;
}

void Game::initDatas() {
    _numerics = loadNumericsFromJson();
    _units = loadUnitsFromJson();
    _views = loadViewsFromJson();
}

void Game::initAddrs() {
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

int Game::hasPlayer() {
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

void Game::showInfo() {
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

void Game::showNames() {
    for (auto& it : _numerics) {
        std::cout << it.getName() << std::endl;
    }
    for (auto& it : _units) {
        std::cout << it.getName() << std::endl;
    }
    std::cout << _strName.getName() << std::endl;
}

int Game::getHandle() {
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

uint32_t Game::getAddr(uint32_t offset) {
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
