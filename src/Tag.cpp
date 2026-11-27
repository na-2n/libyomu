#include "libyomu/Tag.hpp"

#include "libyomu/Error.hpp"
#include "util.hpp"

#include <nlohmann/json.hpp>

#include <string>

namespace yomu {
    Tag Tag::fromJson(const nlohmann::json& arr)
    {
        enum FieldIndices { // NOLINT(*-use-enum-class)
            NAME = 0,
            CATEGORY = 1,
            ORDER = 2,
            NOTES = 3,
            SCORE = 4,
            FIELD_COUNT,
        };

        if (!arr.is_array() || arr.size() != FIELD_COUNT) {
            throw DictionaryError{"Invalid tag entry: {}", arr.dump()};
        }

        auto tag = Tag{};

        arr.at(NAME).get_to(tag._name);
        arr.at(CATEGORY).get_to(tag._category);
        tag._order = util::toInt(arr.at(ORDER));
        arr.at(NOTES).get_to(tag._notes);
        tag._score = util::toInt(arr.at(SCORE));

        return tag;
    }

    Tag Tag::fromJson(std::string_view name, const nlohmann::json& obj)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid legacy tag entry for '{}': {}", name, obj.dump()};
        }

        auto tag = Tag{};

        tag._name = name;
        tag._category = obj.value("category", "");
        tag._order = obj.contains("order") ? util::toInt(obj.at("order")) : 0;
        tag._notes = obj.value("notes", "");
        tag._score = obj.contains("score") ? util::toInt(obj.at("score")) : 0;

        return tag;
    }
} // namespace yomu
