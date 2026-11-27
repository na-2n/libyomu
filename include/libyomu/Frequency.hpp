#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <string>

namespace yomu {
    // Combines the number, string and {value, displayValue} frequency forms
    class Frequency {
    private:
        double _value = 0;
        std::optional<std::string> _displayValue;

    public:
        Frequency() = default;

        [[nodiscard]]
        inline double value() const { return _value; }

        [[nodiscard]]
        inline const std::optional<std::string>& displayValue() const { return _displayValue; }

        friend void from_json(const nlohmann::json& j, Frequency& f);
    };

    void from_json(const nlohmann::json& j, Frequency& f);
} // namespace yomu
