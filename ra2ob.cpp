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
#define NAMEOFFSET 0x34
#define BUILDINGOFFSET 0x5554
#define UNITOFFSET 0x5568
#define INFANTRYOFFSET 0x557c
#define AIRCRAFTOFFSET 0x5590

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

    using Datas = std::vector<DataBase>;

    Datas loadNumericsFromJson(std::string filePath = "../numeric_offsets.json") {
      Datas numerics;

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

    Datas loadUnitsFromJson(std::string filePath = "../unit_offsets.json") {
      Datas units;

      std::ifstream f(filePath);
      json data = json::parse(f);

      for (auto& ft : data.items()) {

        Unit::factionType s_ft;

        if (ft.key() == "Soviet") {
          s_ft = Unit::Soviet;
        } else if (ft.key() == "Allied") {
          s_ft = Unit::Allied;
        } else {
          s_ft = Unit::UnknownFaction;
        } 

        for (auto& ut : data[ft.key()].items()) {

          Unit::unitType s_ut;

          if (ut.key() == "Building") {
            s_ut = Unit::Building;
          } else if (ut.key() == "Tank") {
            s_ut = Unit::Tank;
          } else if (ut.key() == "Infantry") {
            s_ut = Unit::Infantry;
          } else if (ut.key() == "Aircraft") {
            s_ut = Unit::Aircraft;
          } else {
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

    void initDatas() {
      // Datas u = loadUnitsFromJson();
      Datas n = loadNumericsFromJson();
      StrName sn("Player Name", 0x24);

      // _datas.insert(_datas.end(), u.begin(), u.end());
      _datas.insert(_datas.end(), n.begin(), n.end());
      _datas.push_back(sn);
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

          std::vector<uint32_t> playerAddr = {
            realPlayerBase,
            getAddr(realPlayerBase + BUILDINGOFFSET),
            getAddr(realPlayerBase + UNITOFFSET),
            getAddr(realPlayerBase + INFANTRYOFFSET),
            getAddr(realPlayerBase + AIRCRAFTOFFSET),
            getAddr(realPlayerBase + NAMEOFFSET)
          };

          _playerAddrs.push_back(playerAddr);
        } 
      }
    }

    void showInfo() {
      for (auto& it : _datas) {
        it.showInfo();
      }
    }

    void showNames() {
      for (auto& it : _datas) {
        std::cout << it.getName() << std::endl;
      }
    }

    // void showNums() {
    //   for (auto& it : _datas) {
    //     std::cout << it.getName() << ": " << std::dec << it.getValue() << std::endl;
    //   }
    // }

    int getHandle() {
      DWORD pid;
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

    int refreshData() {
      if (NULL == _pHandle) {
        std::cerr << "No valid process handle, call Game::getHandle() first." << std::endl;
        return 1;
      }

      for (auto& it : _datas) {
        // if (Unit u = dynamic_cast<Unit>(it)) {
        //   if (u.getUnitType() == Unit::Building) {
        //     std::cout << "yes";
        //   }
        // }
      }
      return 0;
    }

    HANDLE _pHandle;
    Datas _datas;

    std::vector<std::vector<uint32_t>> _playerAddrs;

};


int main() {
  Game g = Game();
  g.initDatas();

  if (g.getHandle() == 0) {
    g.initAddrs();

    std::cout << "Player numbers: " << g._playerAddrs.size() << std::endl;
    std::cout << "Info numbers: " << g._playerAddrs[0].size() << std::endl;

    // for (auto& p : g._playerAddrs) {
    //   for (auto& it : g._datas) {
    //     it.fetchData(g._pHandle, p);
    //   }
    //   g.showInfo();
    // }
  }

  return 0;
}