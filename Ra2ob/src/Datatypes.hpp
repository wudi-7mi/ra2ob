#ifndef RA2OB_SRC_DATATYPES_HPP_
#define RA2OB_SRC_DATATYPES_HPP_

#include <Windows.h>

#include <codecvt>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "./Reader.hpp"
#include "./Utils.hpp"

namespace Ra2ob {

struct tagPanelInfo {
    std::string playerName    = "";
    std::string playerNameUtf = "";
    int balance               = 0;
    int creditSpent           = 0;
    int powerDrain            = 0;
    int powerOutput           = 0;
    std::string color         = "#FFFFFF";
    std::string country       = "";
};

struct tagUnitSingle {
    std::string unitName = "";
    int num              = 0;
    int index            = -1;
};

struct tagUnitsInfo {
    std::vector<tagUnitSingle> units;
};

struct tagBuildingNode {
    std::string name;
    int number   = 0;
    int progress = 0;
    int status   = 0;  // 0 - in queue, 1 - stopped, 2 - building.

    explicit tagBuildingNode(std::string n) { name = n; }

    void setStop() { status = 1; }

    void setStart() { status = 2; }
};

struct tagBuildingInfo {
    std::vector<tagBuildingNode> list;
};

struct tagPlayer {
    bool valid = false;
    tagPanelInfo panel;
    tagUnitsInfo units;
    tagBuildingInfo building;
};

struct tagDebugInfo {
    std::vector<uint32_t> playerBase;
    std::vector<uint32_t> buildingBase;
    std::vector<uint32_t> infantryBase;
    std::vector<uint32_t> tankBase;
    std::vector<uint32_t> aircraftBase;
    std::vector<uint32_t> houseType;
};

struct tagGameInfo {
    bool valid = false;
    std::vector<tagPlayer> players;
    tagDebugInfo debug;
};

class Base {
public:
    Base(std::string name, uint32_t offset);
    virtual ~Base();

    std::string getName();
    uint32_t getValueByIndex(int index);
    void setValueByIndex(int index, uint32_t value);
    void fetchData(Reader r, std::vector<uint32_t> baseOffsets);
    bool validIndex(int index);

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
    Unit(std::string name, uint32_t offset, UnitType ut);
    ~Unit();

    UnitType getUnitType();
    void fetchData(Reader r, std::vector<uint32_t> baseOffsets, std::vector<uint32_t> valids);

protected:
    UnitType m_unitType;
};

class StrName : public Base {
public:
    explicit StrName(std::string name = "Player Name", uint32_t offset = STRNAMEOFFSET);
    ~StrName();

    std::string getValueByIndex(int index);
    std::string getValueByIndexUtf(int index);
    void setValueByIndex(int index, std::string value);
    void fetchData(Reader r, std::vector<uint32_t> baseOffsets);

protected:
    std::vector<std::string> m_value;
    std::vector<std::string> m_value_utf;
};

class StrCountry : public StrName {
public:
    explicit StrCountry(std::string name = "Country", uint32_t offset = STRCOUNTRYOFFSET);
    ~StrCountry();

    void fetchData(Reader r, std::vector<uint32_t> baseOffsets);
};

struct tagNumerics {
    std::vector<Numeric> items;

    Numeric getItem(std::string query) {
        for (auto& it : items) {
            if (it.getName() == query) {
                return it;
            }
        }
    }
};

struct tagUnits {
    std::vector<Unit> items;

    Unit getItem(std::string query) {
        for (auto& it : items) {
            if (it.getName() == query) {
                return it;
            }
        }
    }
};

/**
 * Source Code
 */

Base::Base(std::string name, uint32_t offset) {
    m_name   = name;
    m_offset = offset;
    m_value  = std::vector<uint32_t>(MAXPLAYER, 0);
    m_size   = NUMSIZE;
}

Base::~Base() {}

std::string Base::getName() { return m_name; }

uint32_t Base::getValueByIndex(int index) {
    if (validIndex(index)) {
        return m_value[index];
    }
    return -1;
}

void Base::setValueByIndex(int index, uint32_t value) {
    if (validIndex(index)) {
        m_value[index] = value;
    }
}

void Base::fetchData(Reader r, std::vector<uint32_t> baseOffsets) {
    for (int i = 0; i < baseOffsets.size(); i++) {
        if (baseOffsets[i] == 0) {
            continue;
        }

        uint32_t buf = 0;

        r.readMemory(baseOffsets[i] + m_offset, &buf, m_size);
        m_value[i] = buf;
    }
}

bool Base::validIndex(int index) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be larger than MAXPLAYER.\n";
        return false;
    }
    return true;
}

Numeric::Numeric(std::string name, uint32_t offset) : Base(name, offset) {}

Numeric::~Numeric() {}

Unit::Unit(std::string name, uint32_t offset, UnitType ut) : Base(name, offset) { m_unitType = ut; }

Unit::~Unit() {}

void Unit::fetchData(Reader r, std::vector<uint32_t> baseOffsets, std::vector<uint32_t> valids) {
    for (int i = 0; i < baseOffsets.size(); i++) {
        if (baseOffsets[i] == 0) {
            continue;
        }

        if (valids.size() == 0 || m_offset >= valids[i] * 4) {
            m_value[i] = 0;
            continue;
        }

        uint32_t buf = 0;

        r.readMemory(baseOffsets[i] + m_offset, &buf, m_size);
        m_value[i] = buf;
    }
}

UnitType Unit::getUnitType() { return m_unitType; }

StrName::StrName(std::string name, uint32_t offset) : Base(name, offset) {
    m_value     = std::vector<std::string>(MAXPLAYER, "");
    m_value_utf = std::vector<std::string>(MAXPLAYER, "");
    m_size      = STRNAMESIZE;
}

StrName::~StrName() {}

void StrName::fetchData(Reader r, std::vector<uint32_t> baseOffsets) {
    for (int i = 0; i < baseOffsets.size(); i++) {
        if (baseOffsets[i] == 0) {
            continue;
        }

        wchar_t buf[STRNAMESIZE] = L"";
        r.readMemory(baseOffsets[i] + m_offset, &buf, m_size);

        m_value[i]     = utf16ToGbk(buf);
        m_value_utf[i] = utf16ToUtf8(buf);
    }
}

std::string StrName::getValueByIndexUtf(int index) {
    if (validIndex(index)) {
        return m_value_utf[index];
    }
    return "";
}

std::string StrName::getValueByIndex(int index) {
    if (validIndex(index)) {
        return m_value[index];
    }
    return "";
}

void StrName::setValueByIndex(int index, std::string value) {
    if (validIndex(index)) {
        m_value[index] = value;
    }
}

StrCountry::StrCountry(std::string name, uint32_t offset) : StrName(name, offset) {
    m_size = STRCOUNTRYSIZE;
}

StrCountry::~StrCountry() {}

void StrCountry::fetchData(Reader r, std::vector<uint32_t> baseOffsets) {
    for (int i = 0; i < baseOffsets.size(); i++) {
        if (baseOffsets[i] == 0) {
            continue;
        }

        char buf[STRCOUNTRYSIZE] = "\0";

        r.readMemory(baseOffsets[i] + m_offset, &buf, m_size);

        auto it = COUNTRYMAP.find(buf);
        if (it == COUNTRYMAP.end()) {
            m_value[i] = "";
        } else {
            m_value[i] = it->second;
        }
    }
}

}  // end of namespace Ra2ob

#endif  // RA2OB_SRC_DATATYPES_HPP_
