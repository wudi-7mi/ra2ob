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

#ifndef RA2OB_HPP_
#define RA2OB_HPP_

#include <Windows.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "./json.hpp"
#include "spdlog/spdlog.h"

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

#define WINOFFSET 0x1f7
#define LOSEOFFSET 0x1f8

#define UNITSAFE 4096

using json = nlohmann::json;

class Ra2ob {
public:
    static Ra2ob& getInstance();

    Ra2ob(const Ra2ob&)          = delete;
    void operator=(const Ra2ob&) = delete;

    enum class FactionType : int { Soviet = 2, Allied = 1, Unknown = 0 };
    enum class UnitType : int { Building = 4, Tank = 3, Infantry = 2, Aircraft = 1, Unknown = 0 };
    enum class ViewType : int { Auto = 0, Manual = 1, ManualNoZero = 2 };

    class View {
    public:
        explicit View(std::string jsonFile = "./view.json");
        ~View();

        void loadFromJson(std::string jsonFile);
        void refreshView(std::string key, std::string value, int index);
        void sortView();
        void viewPrint();
        json viewToJson();
        json viewToJsonFull();
        json getPlayerPanelInfo(int index);
        json getPlayerUnitInfo(int index);

        json m_numericView, m_unitView, m_order, m_validPlayer;
        bool m_gameValid;
        ViewType m_viewType;
    };

    class Base {
    public:
        Base(std::string name, uint32_t offset);
        virtual ~Base();

        void showInfo();
        std::string getName();
        uint32_t getValueByIndex(int index);
        void setValueByIndex(int index, uint32_t value);
        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);

    protected:
        std::string m_name;
        std::vector<uint32_t> m_value;
        uint32_t m_offset;
        uint32_t m_size;
    };

    class Numeric : public Base {
    public:
        Numeric(std::string name, uint32_t offset);
        ~Numeric();
    };

    class Unit : public Base {
    public:
        Unit(std::string name, uint32_t offset, FactionType ft, UnitType ut);
        ~Unit();

        FactionType getFactionType();
        UnitType getUnitType();

    protected:
        FactionType m_factionType;
        UnitType m_unitType;
    };

    class StrName : public Base {
    public:
        explicit StrName(std::string name = "Player Name", uint32_t offset = STRNAMEOFFSET);
        ~StrName();

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);
        std::string getValueByIndex(int index);
        void setValueByIndex(int index, std::string value);

    protected:
        std::vector<std::string> m_value;
    };

    class StrCountry : public StrName {
    public:
        explicit StrCountry(std::string name = "Country", uint32_t offset = STRCOUNTRYOFFSET);
        ~StrCountry();

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);

    protected:
        std::map<std::string, std::string> m_countryMap = {
            {"Americans", "Americans"}, {"Alliance", "Korea"},    {"French", "French"},
            {"Germans", "Germans"},     {"British", "British"},   {"Africans", "Libya"},
            {"Arabs", "Iraq"},          {"Russians", "Russians"}, {"Confederation", "Cuba"}};
    };

    class WinOrLose : public Base {
    public:
        WinOrLose(std::string name, uint32_t offset);
        ~WinOrLose();

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);
        bool getValueByIndex(int index);
        void setValueByIndex(int index, bool value);

    protected:
        std::vector<bool> m_value;
    };

    using Numerics   = std::vector<Numeric>;
    using Units      = std::vector<Unit>;
    using WinOrLoses = std::vector<WinOrLose>;

    Numerics loadNumericsFromJson(std::string filePath = "./panel_offsets.json");
    Units loadUnitsFromJson(std::string filePath = "./unit_offsets.json");
    WinOrLoses initWinOrLose();
    void initDatas();
    bool initAddrs();
    int hasPlayer();
    bool refreshInfo();
    void updateView(bool show = true);
    int getHandle(bool show = true);
    uint32_t getAddr(uint32_t offset);
    FactionType countryToFaction(std::string country);
    static bool readMemory(HANDLE handle, uint32_t addr, void* value, uint32_t size);
    std::string getTime();

    void close();
    void detectTask(bool show = true, int interval = 500);
    void fetchTask(int interval = 500);
    void refreshViewTask(bool show = true, int interval = 500);
    void startLoop(bool show = true);

    std::shared_ptr<spdlog::logger> _logger;
    bool _gameValid;
    HANDLE _pHandle;
    Numerics _numerics;
    Units _units;
    WinOrLoses _winOrLoses;
    StrName _strName;
    StrCountry _strCountry;
    View _view;

    std::vector<bool> _players;
    std::vector<uint32_t> _playerBases;
    std::vector<uint32_t> _buildings;
    std::vector<uint32_t> _infantrys;
    std::vector<uint32_t> _tanks;
    std::vector<uint32_t> _aircrafts;
    std::vector<uint32_t> _houseTypes;
    std::vector<FactionType> _factionTypes;

private:
    Ra2ob();
    ~Ra2ob();
};

#endif  // RA2OB_HPP_
