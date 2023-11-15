#ifndef RA2OB_SRC_FILEIO_HPP_
#define RA2OB_SRC_FILEIO_HPP_

#include <fstream>
#include <iostream>
#include <string>

#include "./json.hpp"

using json = nlohmann::json;

namespace Ra2ob {

class FileIO {
public:
    FileIO();
    json readJson(std::string filePath);
};

FileIO::FileIO() {}

json FileIO::readJson(std::string filePath) {
    std::ifstream f(filePath);
    json j;

    try {
        j = json::parse(f);
    } catch (json::parse_error err) {
        std::cerr << filePath << " parse error.\n";
        std::exit(1);
    }

    return j;
}

}  // namespace Ra2ob

#endif  // RA2OB_SRC_FILEIO_HPP_
