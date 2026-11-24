#include "util.hpp"

#include <string>
#include <vector>

namespace yomu::util {
    std::vector<std::string> strsplit(std::string_view str, std::string_view delim)
    {
        auto parts = std::vector<std::string>{};

        size_t lastPos = 0;
        size_t pos = 0;
        while ((pos = str.find(delim)) != std::string::npos) {
            auto part = str.substr(lastPos, pos);
            parts.push_back(std::string{part});

            lastPos = pos + delim.length();
        }

        return parts;
    }

    std::vector<std::string_view> strsplit_view(std::string_view str, std::string_view delim)
    {
        auto parts = std::vector<std::string_view>{};

        size_t lastPos = 0;
        size_t pos = 0;
        while ((pos = str.find(delim)) != std::string::npos) {
            auto part = str.substr(lastPos, pos);
            parts.push_back(part);

            lastPos = pos + delim.length();
        }

        return parts;
    }
}

