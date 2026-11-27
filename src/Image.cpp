#include "libyomu/Image.hpp"

#include "libyomu/Error.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

namespace yomu {
    Image::Image(bool collapsible)
        : _collapsible{collapsible}
    {}

    Image::Image(std::string_view path, bool collapsible)
        : _path{path}, _collapsible{collapsible}
    {}

    void from_json(const nlohmann::json& obj, Image& img)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"Invalid image: {}", obj.dump()};
        }

        if (!obj.contains("path")) {
            throw DictionaryError{"Image is missing the required 'path' field: {}", obj.dump()};
        }

        obj.at("path").get_to(img._path);

        for (auto [key, field] :
             {std::pair{"width", &img._width}, std::pair{"height", &img._height}}) {
            if (obj.contains(key)) {
                *field = obj.at(key).get<double>();
            }
        }

        for (auto [key, field] : {std::pair{"title", &img._title}, std::pair{"alt", &img._alt}}) {
            if (obj.contains(key)) {
                *field = obj.at(key).get<std::string>();
            }
        }

        if (obj.contains("pixelated")) {
            if (obj.at("pixelated").get<bool>()) {
                img._rendering = Image::RenderingType::Pixelated;
            }
        }

        if (obj.contains("imageRendering")) {
            auto type = obj.at("imageRendering").get<std::string>();

            img._rendering = type == "auto"          ? Image::RenderingType::Auto
                             : type == "pixelated"   ? Image::RenderingType::Pixelated
                             : type == "crisp-edges" ? Image::RenderingType::CrispEdges
                                                     : Image::RenderingType::Auto;
        }

        if (obj.contains("appearance")) {
            auto type = obj.at("appearance").get<std::string>();

            img._appearance = type == "auto"         ? Image::AppearanceType::Auto
                              : type == "monochrome" ? Image::AppearanceType::Monochrome
                                                     : Image::AppearanceType::Auto;
        }

        if (obj.contains("background")) {
            obj.at("background").get_to(img._showBackground);
        }

        if (obj.contains("collapsed")) {
            obj.at("collapsed").get_to(img._collapsed);
        }

        if (obj.contains("collapsible")) {
            obj.at("collapsible").get_to(img._collapsible);
        }
    }
} // namespace yomu
