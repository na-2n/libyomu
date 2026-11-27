#include "util.hpp"

#include <ranges>
#include <string>
#include <vector>

namespace yomu::util {
    std::vector<std::string_view> strsplit_view(std::string_view str, std::string_view delim)
    {
        auto parts = std::vector<std::string_view>{};

        for (auto part : str | std::views::split(delim)) {
            if (!part.empty()) {
                parts.emplace_back(part.begin(), part.end());
            }
        }

        return parts;
    }

    std::vector<std::string> strsplit(std::string_view str, std::string_view delim)
    {
        auto parts = std::vector<std::string>{};

        for (auto part : strsplit_view(str, delim)) {
            parts.emplace_back(part);
        }

        return parts;
    }
} // namespace yomu::util
