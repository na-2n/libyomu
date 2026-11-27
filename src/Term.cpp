#include "libyomu/Term.hpp"

#include "libyomu/Error.hpp"
#include "util.hpp"

#include <nlohmann/json.hpp>

#include <ranges>
#include <string>

namespace yomu {
    Term Term::fromJson(const nlohmann::json& obj, int format)
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

        if (!obj.is_array() || obj.size() < GLOSSARY) {
            throw DictionaryError{"Invalid term entry: {}", obj.dump()};
        }

        auto term = Term{};

        term._expression = obj.at(EXPRESSION);
        term._reading = obj.at(READING);

        // An empty reading means it is the same as the expression
        if (term._reading.empty()) {
            term._reading = term._expression;
        }

        if (const auto& defTags = obj.at(DEFINITION_TAGS); !defTags.is_null()) {
            auto str = defTags.get<std::string>();
            term._defTags = util::strsplit(str, " ");
        }

        term._rules = util::strsplit(obj.at(RULES).get<std::string>(), " ");
        term._score = obj.at(SCORE);

        switch (format) {
            case 1: {
                for (const auto& gloss : obj | std::views::drop(GLOSSARY)) {
                    term._glossary.push_back(gloss.get<Glossary>());
                }
            } break;

            case 2:
            case 3: {
                for (const auto& gloss : obj.at(GLOSSARY)) {
                    term._glossary.push_back(gloss.get<Glossary>());
                }
                if (auto seq = obj.at(SEQUENCE).get<int>(); seq >= 0) {
                    term._sequenceNum = seq;
                }
                term._termTags = util::strsplit(obj.at(TERM_TAGS).get<std::string>(), " ");
            } break;

            default:
                throw DictionaryError{"Unsupported dictionary format: {}", format};
        }

        return term;
    }
} // namespace yomu
