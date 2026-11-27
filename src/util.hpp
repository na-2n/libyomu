#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace yomu::util {
    std::vector<std::string_view> strsplit_view(std::string_view str, std::string_view delim);

    std::vector<std::string> strsplit(std::string_view str, std::string_view delim);

    // Range checked number to int conversion, fractions are truncated.
    // nlohmann's own conversion is undefined behaviour for out of range floats.
    int toInt(const nlohmann::json& obj);

    // Combines the single integer and integer array forms
    std::vector<int> toIntVector(const nlohmann::json& obj);
} // namespace yomu::util
