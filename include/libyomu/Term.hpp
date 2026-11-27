#pragma once

#include "libyomu/Glossary.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace yomu {
    class Term {
    private:
        std::string _expression;
        std::string _reading;
        std::vector<std::string> _defTags;
        std::vector<std::string> _rules;
        int _score = 0;
        std::vector<Glossary> _glossary;
        int _sequenceNum = -1;
        std::vector<std::string> _termTags;

    public:
        Term() = default;

        static constexpr std::string_view FileMatch = "term_bank_";

        [[nodiscard]]
        inline const std::string& expression() const
        {
            return _expression;
        }

        [[nodiscard]]
        inline const std::string& reading() const
        {
            return _reading;
        }

        [[nodiscard]]
        const std::vector<std::string>& definitionTags() const
        {
            return _defTags;
        }

        [[nodiscard]]
        const std::vector<std::string>& rules() const
        {
            return _rules;
        }

        [[nodiscard]]
        inline int score() const
        {
            return _score;
        }

        [[nodiscard]]
        const std::vector<Glossary>& glossary() const
        {
            return _glossary;
        }

        [[nodiscard]]
        inline int sequenceNum() const
        {
            return _sequenceNum;
        }

        [[nodiscard]]
        const std::vector<std::string>& termTags() const
        {
            return _termTags;
        }

        static Term fromJson(const nlohmann::json& obj, int format);
    };
} // namespace yomu
