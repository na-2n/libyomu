#include "libyomu/Kanji.hpp"

#include "libyomu/Error.hpp"
#include "util.hpp"

#include <nlohmann/json.hpp>

#include <ranges>
#include <string>

namespace yomu {
    Kanji Kanji::fromJson(const nlohmann::json& arr, int format)
    {
        enum FieldIndices { // NOLINT(*-use-enum-class)
            CHARACTER = 0,
            ONYOMI = 1,
            KUNYOMI = 2,
            TAGS = 3,
            MEANINGS = 4,
            STATS = 5,
        };

        if (!arr.is_array() || arr.size() < MEANINGS) {
            throw DictionaryError{"Invalid kanji entry: {}", arr.dump()};
        }

        auto kanji = Kanji{};

        arr.at(CHARACTER).get_to(kanji._character);
        kanji._onyomi = util::strsplit(arr.at(ONYOMI).get<std::string>(), " ");
        kanji._kunyomi = util::strsplit(arr.at(KUNYOMI).get<std::string>(), " ");
        kanji._tags = util::strsplit(arr.at(TAGS).get<std::string>(), " ");

        switch (format) {
            case 1: {
                for (const auto& meaning : arr | std::views::drop(MEANINGS)) {
                    kanji._meanings.push_back(meaning.get<std::string>());
                }
            } break;

            case 2:
            case 3: {
                arr.at(MEANINGS).get_to(kanji._meanings);
                arr.at(STATS).get_to(kanji._stats);
            } break;

            default:
                throw DictionaryError{"Unsupported dictionary format: {}", format};
        }

        return kanji;
    }

    KanjiMeta KanjiMeta::fromJson(const nlohmann::json& arr)
    {
        enum FieldIndices { // NOLINT(*-use-enum-class)
            CHARACTER = 0,
            MODE = 1,
            DATA = 2,
            FIELD_COUNT,
        };

        if (!arr.is_array() || arr.size() != FIELD_COUNT) {
            throw DictionaryError{"Invalid kanji meta entry: {}", arr.dump()};
        }

        if (auto mode = arr.at(MODE).get<std::string>(); mode != "freq") {
            throw DictionaryError{"Unknown kanji meta mode: {}", mode};
        }

        auto meta = KanjiMeta{};

        arr.at(CHARACTER).get_to(meta._character);
        arr.at(DATA).get_to(meta._frequency);

        return meta;
    }
} // namespace yomu
