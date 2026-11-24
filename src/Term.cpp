#include "libyomu/Term.hpp"
#include "util.hpp"

#include <nlohmann/json.hpp>

#include <ranges>
#include <string>

namespace yomu {
    Term Term::fromJson(nlohmann::json& obj, int format)
    {
        enum FieldIndices {
            EXPRESSION = 0,
            READING = 1,
            DEFINITION_TAGS = 2,
            RULES = 3,
            SCORE = 4,
            GLOSSARY = 5,
            SEQUENCE = 6,
            TERM_TAGS = 7,
        };

        auto term = Term{};

        term._expression = obj[EXPRESSION];
        term._reading = obj[READING];

        if (auto defTags = obj[DEFINITION_TAGS]; !defTags.is_null()) {
            auto str = defTags.get<std::string>();
            term._defTags = util::strsplit(str, " ");
        }

        term._rules = util::strsplit(obj[RULES].get<std::string>(), " ");
        term._score = obj[SCORE];

        switch (format) {
            case 1: {
                for (auto gloss : obj | std::views::drop(GLOSSARY)) {
                    term._glossary.push_back(Glossary::fromJson(gloss));
                }
            } break;

            case 2:
            case 3: {
                for (auto gloss : obj[GLOSSARY]) {
                    term._glossary.push_back(Glossary::fromJson(gloss));
                }
                term._sequenceNum = obj[SEQUENCE];
                term._termTags = util::strsplit(obj[TERM_TAGS].get<std::string>(), " ");
            } break;

            default:
                break;
        }

        return term;
    }
} // namespace yomu
