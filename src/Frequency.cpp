#include "libyomu/Frequency.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <cmath>
#include <cstdlib>
#include <regex>
#include <string>

namespace yomu {
    namespace {
        // Value of the first number found in the string, or 0 if there is none
        double parseNumber(const std::string& str)
        {
            static const auto regex = std::regex{R"([+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?)"};

            auto match = std::smatch{};
            if (!std::regex_search(str, match, regex)) {
                return 0;
            }

            auto value = std::strtod(match.str().c_str(), nullptr);
            return std::isfinite(value) ? value : 0;
        }
    } // namespace

    void from_json(const nlohmann::json& obj, Frequency& f)
    {
        if (obj.is_number()) {
            obj.get_to(f._value);
        } else if (obj.is_string()) {
            auto str = obj.get<std::string>();
            f._value = parseNumber(str);
            f._displayValue = std::move(str);
        } else if (obj.is_object()) {
            obj.at("value").get_to(f._value);

            if (obj.contains("displayValue")) {
                f._displayValue = obj.at("displayValue").get<std::string>();
            }
        } else {
            throw DictionaryError{"Invalid frequency data: {}", obj.dump()};
        }
    }
} // namespace yomu
