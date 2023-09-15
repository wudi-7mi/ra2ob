#ifndef RA2OB_H_
#define RA2OB_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <Windows.h>

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

class Game {

public:
    Game();

    class DataBase {

    public:
        DataBase(std::string name, uint32_t offset);

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


    class Numeric : public DataBase {

    public:
        Numeric(std::string name, uint32_t offset);
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
        );

        factionType getFactionType();
        unitType getUnitType();

    protected:
        factionType m_factionType;
        unitType m_unitType;
    };


    class StrName : public DataBase {

    public:
        StrName(std::string name, uint32_t offset);

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);
        std::string getValueByIndex(int index);
        void setValueByIndex(int index, std::string value);

    protected:
        std::vector<std::string> m_value;
    };


    class StrCountry : public StrName {

    public:
        StrCountry(std::string name, uint32_t offset);

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);
    
    protected:
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
        WinOrLose(std::string name, uint32_t offset);

        void fetchData(HANDLE pHandle, std::vector<uint32_t> baseOffsets);
        bool getValueByIndex(int index);
        void setValueByIndex(int index, bool value);

    protected:
        std::vector<bool> m_value;
    };

    using Numerics = std::vector<Numeric>;
    using Units = std::vector<Unit>;

    Numerics loadNumericsFromJson(std::string filePath = "../numeric_offsets.json");
    Units loadUnitsFromJson(std::string filePath = "../unit_offsets.json");
    std::vector<std::string> loadViewsFromJson(std::string filePath = "../view.json");
    void initDatas();
    void initAddrs();
    int hasPlayer();
    void showInfo();
    void showNames();
    int getHandle();
    uint32_t getAddr(uint32_t offset);

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

 
#endif