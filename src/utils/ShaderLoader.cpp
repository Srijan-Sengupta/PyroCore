//
// Created by srijan on 2/9/25.
//

#include "ShaderLoader.hpp"

#include <fstream>
#include <iostream>

#include "Logger.hpp"

namespace pyro {
    std::vector<char> ShaderLoader::loadSPV(const std::string &path) {
        std::vector<char> data;
        std::ifstream file(path, std::ios::binary|std::ios::ate);
        if (!file.is_open()) {
            LOG(LogLevel::ERROR, "Failed to open file %s", path);
        }
        size_t file_size = file.tellg();
        if (file_size % 4 != 0) {
            LOG(LogLevel::ERROR, "File size is not a multiple of 4");
        }
        data.resize(file_size);
        file.seekg(0);
        file.read(data.data(), file_size);
        file.close();
        return data;
    }
} // namespace pyro
