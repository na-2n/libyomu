#include "libyomu/Glossary.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>

namespace yomu {
    TextGlossary::TextGlossary(std::string_view text)
        : _text{text}
    {}

    ImageGlossary::ImageGlossary(std::string_view path)
        : _path{path}
    {}

    Glossary::Glossary(GlossaryType type, std::unique_ptr<GlossaryItem> item)
        : _type{type}, _item{std::move(item)}
    {}

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
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid image glossary: {}", obj.dump()};
        }

        if (!obj.contains("path")) {
            throw DictionaryError{"Image glossary is missing the required 'path' field: {}",
                                  obj.dump()};
        }

        obj.at("path").get_to(g._path);

        for (auto [key, field] : {std::pair{"width", &g._width}, std::pair{"height", &g._height}}) {
            if (obj.contains(key)) {
                *field = obj.at(key).get<int>();
            }
        }

        for (auto [key, field] : {
                 std::pair{"title", &g._title},
                 std::pair{"alt", &g._alt},
                 std::pair{"description", &g._description},
             }) {
            if (obj.contains(key)) {
                *field = obj.at(key).get<std::string>();
            }
        }

        if (obj.contains("pixelated")) {
            if (obj.at("pixelated").get<bool>()) {
                g._rendering = ImageGlossary::RenderingType::Pixelated;
            }
        }

        if (obj.contains("imageRendering")) {
            auto type = obj.at("imageRendering").get<std::string>();

            g._rendering = type == "auto"          ? ImageGlossary::RenderingType::Auto
                           : type == "pixelated"   ? ImageGlossary::RenderingType::Pixelated
                           : type == "crisp-edges" ? ImageGlossary::RenderingType::CrispEdges
                                                   : ImageGlossary::RenderingType::Auto;
        }

        if (obj.contains("appearance")) {
            auto type = obj.at("appearance").get<std::string>();

            g._appearance = type == "auto"         ? ImageGlossary::AppearanceType::Auto
                            : type == "monochrome" ? ImageGlossary::AppearanceType::Monochrome
                                                   : ImageGlossary::AppearanceType::Auto;
        }

        if (obj.contains("background")) {
            obj.at("background").get_to(g._showBackground);
        }

        if (obj.contains("collapsed")) {
            obj.at("collapsed").get_to(g._collapsed);
        }

        if (obj.contains("collapsible")) {
            obj.at("collapsible").get_to(g._collapsible);
        }
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

            default:
                g._item = nullptr;
                break;
        }
    }
} // namespace yomu
