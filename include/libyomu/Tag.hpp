#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <string_view>

namespace yomu {
    class Tag {
    private:
        std::string _name;
        std::string _category;
        int _order = 0;
        std::string _notes;
        int _score = 0;

    public:
        Tag() = default;

        inline static const std::string FileMatch = "tag_bank_";

        [[nodiscard]]
        inline const std::string& name() const { return _name; }

        [[nodiscard]]
        inline const std::string& category() const { return _category; }

        [[nodiscard]]
        inline int order() const { return _order; }

        [[nodiscard]]
        inline const std::string& notes() const { return _notes; }

        [[nodiscard]]
        inline int score() const { return _score; }

        // Tag bank row: [name, category, order, notes, score]
        static Tag fromJson(const nlohmann::json& arr);

        // Legacy index.json `tagMeta` entry: name -> {category, order, notes, score}
        static Tag fromJson(std::string_view name, const nlohmann::json& obj);
    };
} // namespace yomu
