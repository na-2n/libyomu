#include "libyomu/TermMeta.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace yomu {
    namespace {
        // Combines the single integer and integer array forms
        std::vector<int> toIntVector(const nlohmann::json& obj)
        {
            if (obj.is_number_integer()) {
                return {obj.get<int>()};
            }

            if (obj.is_array()) {
                return obj.get<std::vector<int>>();
            }

            throw DictionaryError{"Expected integer or integer array: {}", obj.dump()};
        }
    } // namespace

    void from_json(const nlohmann::json& obj, FreqTermMeta& m)
    {
        if (obj.is_object() && obj.contains("reading")) {
            m._reading = obj.at("reading").get<std::string>();
            obj.at("frequency").get_to(m._frequency);
        } else {
            obj.get_to(m._frequency);
        }
    }

    void from_json(const nlohmann::json& obj, PitchTermMeta::Pitch& p)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid pitch entry: {}", obj.dump()};
        }

        if (auto& pos = obj.at("position"); pos.is_number_integer()) {
            p._position = pos.get<int>();
        } else if (pos.is_string()) {
            auto pattern = pos.get<std::string>();

            if (pattern.empty() ||
                !std::ranges::all_of(pattern, [](char c) { return c == 'H' || c == 'L'; })) {
                throw DictionaryError{"Invalid pitch pattern: {}", pattern};
            }

            p._position = std::move(pattern);
        } else {
            throw DictionaryError{"Invalid pitch position: {}", pos.dump()};
        }

        if (obj.contains("nasal")) {
            p._nasal = toIntVector(obj.at("nasal"));
        }

        if (obj.contains("devoice")) {
            p._devoice = toIntVector(obj.at("devoice"));
        }

        if (obj.contains("tags")) {
            obj.at("tags").get_to(p._tags);
        }
    }

    void from_json(const nlohmann::json& obj, PitchTermMeta& m)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid pitch data: {}", obj.dump()};
        }

        obj.at("reading").get_to(m._reading);
        obj.at("pitches").get_to(m._pitches);
    }

    void from_json(const nlohmann::json& obj, IpaTermMeta::Transcription& t)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid IPA transcription: {}", obj.dump()};
        }

        obj.at("ipa").get_to(t.ipa);

        if (obj.contains("tags")) {
            obj.at("tags").get_to(t.tags);
        }
    }

    void from_json(const nlohmann::json& obj, IpaTermMeta& m)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid IPA data: {}", obj.dump()};
        }

        obj.at("reading").get_to(m._reading);
        obj.at("transcriptions").get_to(m._transcriptions);
    }

    TermMeta TermMeta::fromJson(const nlohmann::json& arr)
    {
        enum FieldIndices { // NOLINT(*-use-enum-class)
            EXPRESSION = 0,
            MODE = 1,
            DATA = 2,
            FIELD_COUNT,
        };

        if (!arr.is_array() || arr.size() != FIELD_COUNT) {
            throw DictionaryError{"Invalid term meta entry: {}", arr.dump()};
        }

        auto meta = TermMeta{};

        arr.at(EXPRESSION).get_to(meta._expression);

        auto mode = arr.at(MODE).get<std::string>();
        meta._type = mode == "freq"    ? TermMetaType::Frequency
                     : mode == "pitch" ? TermMetaType::Pitch
                     : mode == "ipa"   ? TermMetaType::IPA
                                       : TermMetaType::Unknown;

        auto& data = arr.at(DATA);
        switch (meta._type) {
            case TermMetaType::Frequency:
                meta._item = std::make_unique<FreqTermMeta>(data.get<FreqTermMeta>());
                break;

            case TermMetaType::Pitch:
                meta._item = std::make_unique<PitchTermMeta>(data.get<PitchTermMeta>());
                break;

            case TermMetaType::IPA:
                meta._item = std::make_unique<IpaTermMeta>(data.get<IpaTermMeta>());
                break;

            default:
                throw DictionaryError{"Unknown term meta mode: {}", mode};
        }

        return meta;
    }
} // namespace yomu
