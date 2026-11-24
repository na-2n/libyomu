#include "libyomu/Glossary.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>

namespace yomu {
    TextGlossary::TextGlossary(std::string_view text)
        : _text{text}
    {}

    Glossary::Glossary(GlossaryType type, std::unique_ptr<GlossaryItem> item)
        : _type{type}, _item{std::move(item)}
    {}

    Glossary Glossary::fromJson(const nlohmann::json& o)
    {
        GlossaryType glossType;
        std::unique_ptr<GlossaryItem> gloss = nullptr;

        if (o.is_string()) {
            glossType = GlossaryType::Text;

            auto text = o.get<std::string>();
            gloss = std::make_unique<TextGlossary>(text);
        } else if (o.is_object()) {
            auto type = o["type"].get<std::string>();

            if (type == "text") {
                glossType = GlossaryType::Text;

                auto text = o["text"].get<std::string>();
                gloss = std::make_unique<TextGlossary>(text);
            } else if (type == "image") {
                glossType = GlossaryType::Image;
            } else if (type == "structured-content") {
                glossType = GlossaryType::Structured;
            } else {
                throw; // TODO: proper exception
            }
        } else if (o.is_array()) { // deinflection
            glossType = GlossaryType::Deinflection;
        } else {
            throw; // TODO: proper exception
        }

        return {glossType, std::move(gloss)};
    }
} // namespace yomu
