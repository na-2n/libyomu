#include "libyomu/Frequency.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <cmath>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>

namespace yomu {
    namespace {
        inline bool isDigit(char c)
        {
            return c >= '0' && c <= '9';
        }

        // Value of the first number found in the string, or 0 if there is none.
        // Same as Yomitan's /[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?/, but independent of the C
        // locale.
        double parseNumber(std::string_view str)
        {
            auto start = std::string_view::npos;
            for (size_t i = 0; i < str.size(); ++i) {
                if (isDigit(str[i]) ||
                    (str[i] == '.' && i + 1 < str.size() && isDigit(str[i + 1]))) {
                    start = i;
                    break;
                }
            }

            if (start == std::string_view::npos) {
                return 0;
            }

            if (start > 0 && (str[start - 1] == '+' || str[start - 1] == '-')) {
                --start;
            }

            auto end = start;
            auto skipDigits = [&] {
                while (end < str.size() && isDigit(str[end])) {
                    ++end;
                }
            };

            if (str[end] == '+' || str[end] == '-') {
                ++end;
            }
            skipDigits();
            if (end < str.size() && str[end] == '.') {
                ++end;
                skipDigits();
            }

            // Exponent only counts when followed by digits
            if (end < str.size() && (str[end] == 'e' || str[end] == 'E')) {
                auto exp = end + 1;
                if (exp < str.size() && (str[exp] == '+' || str[exp] == '-')) {
                    ++exp;
                }
                if (exp < str.size() && isDigit(str[exp])) {
                    end = exp;
                    skipDigits();
                }
            }

            auto stream = std::istringstream{std::string{str.substr(start, end - start)}};
            stream.imbue(std::locale::classic());

            auto value = 0.0;
            stream >> value;

            return !stream.fail() && std::isfinite(value) ? value : 0;
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
