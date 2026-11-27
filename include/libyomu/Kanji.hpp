#pragma once

#include "libyomu/Frequency.hpp"

#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace yomu {
    class Kanji {
    private:
        std::string _character;
        std::vector<std::string> _onyomi;
        std::vector<std::string> _kunyomi;
        std::vector<std::string> _tags;
        std::vector<std::string> _meanings;
        std::map<std::string, std::string> _stats;

    public:
        Kanji() = default;

        static constexpr std::string_view FileMatch = "kanji_bank_";

        [[nodiscard]]
        inline const std::string& character() const
        {
            return _character;
        }

        [[nodiscard]]
        inline const std::vector<std::string>& onyomi() const
        {
            return _onyomi;
        }

        [[nodiscard]]
        inline const std::vector<std::string>& kunyomi() const
        {
            return _kunyomi;
        }

        [[nodiscard]]
        inline const std::vector<std::string>& tags() const
        {
            return _tags;
        }

        [[nodiscard]]
        inline const std::vector<std::string>& meanings() const
        {
            return _meanings;
        }

        // Stat name (looked up in the tag bank) -> value, empty for format 1
        [[nodiscard]]
        inline const std::map<std::string, std::string>& stats() const
        {
            return _stats;
        }

        static Kanji fromJson(const nlohmann::json& arr, int format);
    };

    class KanjiMeta {
    private:
        std::string _character;
        Frequency _frequency;

    public:
        KanjiMeta() = default;

        static constexpr std::string_view FileMatch = "kanji_meta_bank_";

        [[nodiscard]]
        inline const std::string& character() const
        {
            return _character;
        }

        [[nodiscard]]
        inline const Frequency& frequency() const
        {
            return _frequency;
        }

        static KanjiMeta fromJson(const nlohmann::json& arr);
    };
} // namespace yomu
