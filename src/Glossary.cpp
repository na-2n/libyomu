#include "libyomu/Glossary.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>

namespace yomu {
    void from_json(const nlohmann::json& obj, TextGlossary& g)
    {
        if (obj.is_string()) {
            obj.get_to(g._text);
        } else if (obj.is_object()) {
            obj.at("text").get_to(g._text);
        } else {
            throw DictionaryError{"Invalid text glossary: {}", obj.dump()};
        }
    }

    void from_json(const nlohmann::json& obj, ImageGlossary& g)
    {
        from_json(obj, static_cast<Image&>(g));

        if (obj.contains("description")) {
            g._description = obj.at("description").get<std::string>();
        }
    }

    void from_json(const nlohmann::json& obj, StructuredGlossary& g)
    {
        if (!obj.is_object() || !obj.contains("content")) {
            throw DictionaryError{
                "Structured content glossary is missing the required 'content' field: {}",
                obj.dump()};
        }

        g._content = ContentNode::fromJson(obj.at("content"));
    }

    void from_json(const nlohmann::json& obj, DeinflectionGlossary& g)
    {
        if (!obj.is_array() || obj.size() != 2 || !obj.at(0).is_string() || !obj.at(1).is_array()) {
            throw DictionaryError{"Invalid deinflection glossary: {}", obj.dump()};
        }

        obj.at(0).get_to(g._uninflected);
        obj.at(1).get_to(g._rules);
    }

    void from_json(const nlohmann::json& o, Glossary& g)
    {
        if (o.is_string()) {
            g._type = GlossaryType::Text;
        } else if (o.is_object()) {
            auto type = o.at("type").get<std::string>();

            if (type == "text") {
                g._type = GlossaryType::Text;
            } else if (type == "image") {
                g._type = GlossaryType::Image;
            } else if (type == "structured-content") {
                g._type = GlossaryType::Structured;
            } else {
                throw DictionaryError{"Unknown glossary type: {}", type};
            }
        } else if (o.is_array()) { // deinflection
            g._type = GlossaryType::Deinflection;
        } else {
            throw DictionaryError{"Invalid glossary: {}", o.dump()};
        }

        switch (g._type) {
            case GlossaryType::Text:
                g._item = std::make_unique<TextGlossary>(o.get<TextGlossary>());
                break;

            case GlossaryType::Image:
                g._item = std::make_unique<ImageGlossary>(o.get<ImageGlossary>());
                break;

            case GlossaryType::Structured:
                g._item = std::make_unique<StructuredGlossary>(o.get<StructuredGlossary>());
                break;

            case GlossaryType::Deinflection:
                g._item = std::make_unique<DeinflectionGlossary>(o.get<DeinflectionGlossary>());
                break;

            default:
                g._item = nullptr;
                break;
        }
    }
} // namespace yomu
