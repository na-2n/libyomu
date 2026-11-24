#pragma once

#include "libyomu/Glossary.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace yomu {
    class Term {
    private:
        std::string _expression;
        std::string _reading;
        std::vector<std::string> _defTags;
        std::vector<std::string> _rules;
        int _score;
        std::vector<Glossary> _glossary;
        int _sequenceNum = -1;
        std::vector<std::string> _termTags;

        Term() = default;

        Term(std::string test)
            : _reading{test} {}
    public:
        inline static const std::string FileMatch = "term_bank_";

        inline const std::string& expression() const { return _expression; }

        inline const std::string& reading() const { return _reading; }

        const std::vector<std::string>& definitionTags() const { return _defTags; }

        const std::vector<std::string>& rules() const { return _rules; }

        inline int score() const { return _score; }

        const std::vector<Glossary>& glossary() const { return _glossary; }

        inline int sequenceNum() const { return _sequenceNum; }

        const std::vector<std::string>& termTags() const { return _termTags; }

        static Term fromJson(nlohmann::json& obj, int format);
    };
} // namespace yomu
