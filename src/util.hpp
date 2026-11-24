#pragma once

#include <string>
#include <vector>

namespace yomu::util {
    std::vector<std::string_view> strsplit_view(std::string_view str, std::string_view delim);

    std::vector<std::string> strsplit(std::string_view str, std::string_view delim);
}

