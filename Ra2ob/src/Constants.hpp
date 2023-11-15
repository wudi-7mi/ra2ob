#ifndef RA2OB_SRC_CONSTANTS_HPP_
#define RA2OB_SRC_CONSTANTS_HPP_

#include <map>
#include <string>

namespace Ra2ob {

// Basic game information

const int MAXPLAYER    = 8;
const int INVALIDCLASS = 0xffffffffu;

// Memory size

const int BOOLSIZE        = 0x1;
const int NUMSIZE         = 0x4;
const int PTRSIZE         = 0x4;
const int COLORSIZE       = 0x6;
const int STRNAMESIZE     = 0x20;  // 32 in hexadecimal
const int STRCOUNTRYSIZE  = 0x19;  // 25 in hexadecimal
const int STRUNITNAMESIZE = 0x20;  // 32 in hexadecimal

// Basic offsets

const int FIXEDOFFSET              = 0xa8b230;
const int CLASSBASEARRAYOFFSET     = 0xa8022c;
const int PLAYERBASEARRAYPTROFFSET = 0x1180;
const int HOUSETYPEOFFSET          = 0x34;
const int COLOROFFSET              = 0x56F9;

const int BUILDINGOFFSET = 0x5554;
const int TANKOFFSET     = 0x5568;
const int INFANTRYOFFSET = 0x557c;
const int AIRCRAFTOFFSET = 0x5590;

const int STRNAMEOFFSET    = 0x1602a;
const int STRCOUNTRYOFFSET = 0x24;

const int WINOFFSET  = 0x1f7;
const int LOSEOFFSET = 0x1f8;

const int UNITSAFE = 4096;

// Production Offsets

const int P_BUILDINGFIRSTOFFSET  = 0x53BC;
const int P_BUILDINGSECONDOFFSET = 0x53CC;
const int P_INFANTRYOFFSET       = 0x53B0;
const int P_TANKOFFSET           = 0x53B4;
const int P_SHIPOFFSET           = 0x53B8;

const int P_TIMEOFFSET        = 0x24;
const int P_QUEUEPTROFFSET    = 0x44;
const int P_QUEUELENGTHOFFSET = 0x50;
const int P_STATUSOFFSET      = 0x70;

const int P_NAMEOFFSET = 0x64;

// Color Codes

const int C_YELLOW  = 0xE0D838;
const int C_PURPLE  = 0x9848B8;
const int C_GREEN   = 0x58CC50;
const int C_RED     = 0xF84C48;
const int C_ORANGE  = 0xF8B048;
const int C_PINK    = 0xF8ACE8;
const int C_SKYBLUE = 0x58D4E0;

// Country Map

const std::map<std::string, std::string> COUNTRYMAP = {
    {"Americans", "Americans"}, {"Alliance", "Korea"},    {"French", "French"},
    {"Germans", "Germans"},     {"British", "British"},   {"Africans", "Libya"},
    {"Arabs", "Iraq"},          {"Russians", "Russians"}, {"Confederation", "Cuba"}};

// Enums

enum class UnitType : int { Building = 4, Tank = 3, Infantry = 2, Aircraft = 1, Unknown = 0 };

// Files
const char F_PANELOFFSETS[] = "./config/panel_offsets.json";
const char F_UNITOFFSETS[]  = "./config/unit_offsets.json";
const char F_VIEW[]         = "./config/view.json";

}  // end of namespace Ra2ob

#endif  // RA2OB_SRC_CONSTANTS_HPP_
