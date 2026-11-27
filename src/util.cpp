#include "util.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
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

    int toInt(const nlohmann::json& obj)
    {
        constexpr auto min = std::numeric_limits<int>::min();
        constexpr auto max = std::numeric_limits<int>::max();

        if (obj.is_number_unsigned()) {
            if (auto value = obj.get<std::uint64_t>(); value <= static_cast<std::uint64_t>(max)) {
                return static_cast<int>(value);
            }
        } else if (obj.is_number_integer()) {
            if (auto value = obj.get<std::int64_t>(); value >= min && value <= max) {
                return static_cast<int>(value);
            }
        } else if (obj.is_number_float()) {
            if (auto value = std::trunc(obj.get<double>()); value >= min && value <= max) {
                return static_cast<int>(value);
            }
        } else {
            throw DictionaryError{"Expected a number, got {}", obj.type_name()};
        }

        throw DictionaryError{"Number out of range: {}", obj.dump()};
    }

    std::vector<int> toIntVector(const nlohmann::json& obj)
    {
        if (obj.is_number()) {
            return {toInt(obj)};
        }

        if (!obj.is_array()) {
            throw DictionaryError{"Expected integer or integer array: {}", obj.dump()};
        }

        auto values = std::vector<int>{};
        values.reserve(obj.size());

        for (const auto& value : obj) {
            values.push_back(toInt(value));
        }

        return values;
    }
} // namespace yomu::util
