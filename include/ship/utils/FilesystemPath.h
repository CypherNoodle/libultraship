#pragma once

#include <filesystem>
#include <string>

namespace Ship {
inline std::filesystem::path AbsolutePath(const std::filesystem::path& path) {
#ifdef __SWITCH__
    // libnx device paths are absolute, but libstdc++ uses POSIX path syntax.
    const auto value = path.generic_string();
    const auto device = value.find(":/");
    if (device != std::string::npos && device > 0 && value.find('/') == device + 1) {
        return path;
    }
#endif
    return std::filesystem::absolute(path);
}
}
