#include "libyomu/Term.hpp"
#include "util.hpp"

#include <nlohmann/json.hpp>

#include <ranges>
#include <string>

namespace yomu {
    Term Term::fromJson(nlohmann::json& obj, int format)
    {
        enum FieldIndices { // NOLINT(*-use-enum-class)
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

        term._expression = obj.at(EXPRESSION);
        term._reading = obj.at(READING);

        if (auto defTags = obj.at(DEFINITION_TAGS); !defTags.is_null()) {
            auto str = defTags.get<std::string>();
            term._defTags = util::strsplit(str, " ");
        }

        term._rules = util::strsplit(obj.at(RULES).get<std::string>(), " ");
        term._score = obj.at(SCORE);

        switch (format) {
            case 1: {
                for (auto gloss : obj | std::views::drop(GLOSSARY)) {
                    term._glossary.push_back(gloss.get<Glossary>());
                }
            } break;

            case 2:
            case 3: {
                for (auto gloss : obj.at(GLOSSARY)) {
                    term._glossary.push_back(gloss.get<Glossary>());
                }
                term._sequenceNum = obj.at(SEQUENCE);
                term._termTags = util::strsplit(obj.at(TERM_TAGS).get<std::string>(), " ");
            } break;

            default:
                break;
        }

        return term;
    }
} // namespace yomu
