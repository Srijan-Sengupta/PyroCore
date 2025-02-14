//
// Created by srijan on 2/9/25.
//

#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP
#include <cstdint>

#include <string>
#include <vector>

namespace pyro {

    class ShaderLoader {
    public:
        static std::vector<char> loadSPV(const std::string &path);
    };

} // namespace pyro

#endif // SHADER_LOADER_HPP
