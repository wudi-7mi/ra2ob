#ifndef RA2OB_SRC_DATATYPES_HPP_
#define RA2OB_SRC_DATATYPES_HPP_

#include <Windows.h>

#include <array>
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
    std::string color         = "ffffff";
    std::string country       = "";
};

struct tagUnitSingle {
    std::string unitName = "";
    int num              = 0;
    int index            = 99;
};

struct tagUnitsInfo {
    std::vector<tagUnitSingle> units;
};

struct tagBuildingNode {
    std::string name;
    int number   = 0;
    int progress = 0;
    int status   = 0;  // 0 - building, 1 - stopped.

    explicit tagBuildingNode(std::string n) { name = n; }
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
    std::array<uint32_t, MAXPLAYER> playerBase{};
    std::array<uint32_t, MAXPLAYER> buildingBase{};
    std::array<uint32_t, MAXPLAYER> infantryBase{};
    std::array<uint32_t, MAXPLAYER> tankBase{};
    std::array<uint32_t, MAXPLAYER> aircraftBase{};
    std::array<uint32_t, MAXPLAYER> houseType{};
};

struct tagGameInfo {
    bool valid = false;
    std::array<tagPlayer, MAXPLAYER> players{};
    tagDebugInfo debug;
};

class Base {
public:
    Base(std::string name, uint32_t offset);
    virtual ~Base();

    std::string getName();
    uint32_t getValueByIndex(int index);
    void setValueByIndex(int index, uint32_t value);
    void fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets);
    bool validIndex(int index);

protected:
    std::string m_name;
    std::array<uint32_t, MAXPLAYER> m_value{};
    uint32_t m_offset;
    uint32_t m_size;
};

class Numeric : public Base {
public:
    using Base::Base;
    ~Numeric();
};

class Unit : public Base {
public:
    Unit(std::string name, uint32_t offset, UnitType ut, int index);
    ~Unit();

    UnitType getUnitType();
    bool checkOffset(int offsetCmp, UnitType type) const;
    int getUnitIndex();
    void fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets,
                   const std::array<uint32_t, MAXPLAYER>& valids);

protected:
    UnitType m_unitType;
    int m_unitIndex;
};

class StrName : public Base {
public:
    explicit StrName(std::string name = "Player Name", uint32_t offset = STRNAMEOFFSET);
    ~StrName();

    std::string getValueByIndex(int index);
    std::string getValueByIndexUtf(int index);
    void setValueByIndex(int index, std::string value);
    void fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets);

protected:
    std::array<std::string, MAXPLAYER> m_value{};
    std::array<std::string, MAXPLAYER> m_value_utf{};
};

class StrCountry : public StrName {
public:
    explicit StrCountry(std::string name = "Country", uint32_t offset = STRCOUNTRYOFFSET);
    ~StrCountry();

    void fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets);
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

inline Base::Base(std::string name, uint32_t offset) {
    m_name   = name;
    m_offset = offset;
    m_size   = NUMSIZE;
}

inline Base::~Base() {}

inline std::string Base::getName() { return m_name; }

inline uint32_t Base::getValueByIndex(int index) {
    if (validIndex(index)) {
        return m_value[index];
    }
    return -1;
}

inline void Base::setValueByIndex(int index, uint32_t value) {
    if (validIndex(index)) {
        m_value[index] = value;
    }
}

inline void Base::fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets) {
    for (int i = 0; i < baseOffsets.size(); i++) {
        if (baseOffsets[i] == 0) {
            continue;
        }

        uint32_t buf = 0;

        r.readMemory(baseOffsets[i] + m_offset, &buf, m_size);
        m_value[i] = buf;
    }
}

inline bool Base::validIndex(int index) {
    if (index >= MAXPLAYER) {
        std::cerr << "Error: Index cannot be larger than MAXPLAYER.\n";
        return false;
    }
    return true;
}

inline Numeric::~Numeric() {}

inline Unit::Unit(std::string name, uint32_t offset, UnitType ut, int index) : Base(name, offset) {
    m_unitType  = ut;
    m_unitIndex = index;
}

inline Unit::~Unit() {}

inline void Unit::fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets,
                            const std::array<uint32_t, MAXPLAYER>& valids) {
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

bool Unit::checkOffset(int offsetCmp, UnitType type) const {
    return (offsetCmp == m_offset && type == m_unitType);
}

inline UnitType Unit::getUnitType() { return m_unitType; }

inline int Unit::getUnitIndex() { return m_unitIndex; }

inline StrName::StrName(std::string name, uint32_t offset) : Base(name, offset) {
    m_size = STRNAMESIZE;
}

inline StrName::~StrName() {}

inline void StrName::fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets) {
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

inline std::string StrName::getValueByIndexUtf(int index) {
    if (validIndex(index)) {
        return m_value_utf[index];
    }
    return "";
}

inline std::string StrName::getValueByIndex(int index) {
    if (validIndex(index)) {
        return m_value[index];
    }
    return "";
}

inline void StrName::setValueByIndex(int index, std::string value) {
    if (validIndex(index)) {
        m_value[index] = value;
    }
}

inline StrCountry::StrCountry(std::string name, uint32_t offset) : StrName(name, offset) {
    m_size = STRCOUNTRYSIZE;
}

inline StrCountry::~StrCountry() {}

inline void StrCountry::fetchData(Reader r, const std::array<uint32_t, MAXPLAYER>& baseOffsets) {
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
