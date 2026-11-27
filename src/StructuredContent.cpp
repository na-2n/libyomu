#include "libyomu/StructuredContent.hpp"

#include "libyomu/Error.hpp"
#include "util.hpp"

#include <nlohmann/json.hpp>

#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace yomu {
    namespace {
        template<typename E>
        using EnumValues = std::initializer_list<std::pair<std::string_view, E>>;

        // Unknown values are left unset, like the image rendering and appearance fields
        template<typename E>
        std::optional<E> toEnum(const nlohmann::json& obj, EnumValues<E> values)
        {
            auto str = obj.get<std::string>();

            for (auto& [name, value] : values) {
                if (name == str) {
                    return value;
                }
            }

            return std::nullopt;
        }

        template<typename E>
        void readEnum(const nlohmann::json& obj, std::string_view key, std::optional<E>& field,
                      EnumValues<E> values)
        {
            if (auto it = obj.find(key); it != obj.end()) {
                field = toEnum(*it, values);
            }
        }

        void readString(const nlohmann::json& obj, std::string_view key,
                        std::optional<std::string>& field)
        {
            if (auto it = obj.find(key); it != obj.end()) {
                field = it->get<std::string>();
            }
        }

        constexpr auto verticalAlignValues = EnumValues<ContentStyle::VerticalAlign>{
            {"baseline", ContentStyle::VerticalAlign::Baseline},
            {"sub", ContentStyle::VerticalAlign::Sub},
            {"super", ContentStyle::VerticalAlign::Super},
            {"text-top", ContentStyle::VerticalAlign::TextTop},
            {"text-bottom", ContentStyle::VerticalAlign::TextBottom},
            {"middle", ContentStyle::VerticalAlign::Middle},
            {"top", ContentStyle::VerticalAlign::Top},
            {"bottom", ContentStyle::VerticalAlign::Bottom},
        };

        constexpr auto textDecorationLineValues = EnumValues<ContentStyle::TextDecorationLine>{
            {"underline", ContentStyle::TextDecorationLine::Underline},
            {"overline", ContentStyle::TextDecorationLine::Overline},
            {"line-through", ContentStyle::TextDecorationLine::LineThrough},
        };

        constexpr auto tagValues = EnumValues<ContentTag>{
            {"br", ContentTag::Br},           {"ruby", ContentTag::Ruby},
            {"rt", ContentTag::Rt},           {"rp", ContentTag::Rp},
            {"table", ContentTag::Table},     {"thead", ContentTag::Thead},
            {"tbody", ContentTag::Tbody},     {"tfoot", ContentTag::Tfoot},
            {"tr", ContentTag::Tr},           {"td", ContentTag::Td},
            {"th", ContentTag::Th},           {"span", ContentTag::Span},
            {"div", ContentTag::Div},         {"ol", ContentTag::Ol},
            {"ul", ContentTag::Ul},           {"li", ContentTag::Li},
            {"details", ContentTag::Details}, {"summary", ContentTag::Summary},
            {"img", ContentTag::Img},         {"a", ContentTag::A},
        };

        // Bounds the recursion below (and in the element destructors), deeper input would overflow
        // the stack
        constexpr auto MaxContentDepth = 128;

        thread_local auto contentDepth = 0;

        class DepthGuard {
        public:
            DepthGuard()
            {
                if (contentDepth >= MaxContentDepth) {
                    throw DictionaryError{"Structured content is nested deeper than {} levels",
                                          MaxContentDepth};
                }
                ++contentDepth;
            }

            ~DepthGuard() { --contentDepth; }

            DepthGuard(const DepthGuard&) = delete;
            DepthGuard(DepthGuard&&) = delete;
            DepthGuard& operator=(const DepthGuard&) = delete;
            DepthGuard& operator=(DepthGuard&&) = delete;
        };

        void appendContent(const nlohmann::json& obj, std::vector<ContentNode>& nodes)
        {
            auto guard = DepthGuard{};

            if (obj.is_string()) {
                nodes.emplace_back(obj.get<std::string>());
            } else if (obj.is_array()) {
                for (const auto& child : obj) {
                    appendContent(child, nodes);
                }
            } else if (obj.is_object()) {
                nodes.emplace_back(obj.get<ContentElement>());
            } else {
                throw DictionaryError{"Invalid structured content: {}", obj.dump()};
            }
        }
    } // namespace

    ContentNode::ContentNode(std::string text)
        : _value{std::move(text)}
    {}

    ContentNode::ContentNode(ContentElement element)
        : _value{std::move(element)}
    {}

    std::vector<ContentNode> ContentNode::fromJson(const nlohmann::json& obj)
    {
        auto nodes = std::vector<ContentNode>{};
        appendContent(obj, nodes);

        return nodes;
    }

    void from_json(const nlohmann::json& obj, ContentStyle& s)
    {
        using S = ContentStyle;

        if (!obj.is_object()) {
            throw DictionaryError{"Invalid structured content style: {}", obj.dump()};
        }

        readEnum(obj, "fontStyle", s.fontStyle,
                 {{"normal", S::FontStyle::Normal}, {"italic", S::FontStyle::Italic}});
        readEnum(obj, "fontWeight", s.fontWeight,
                 {{"normal", S::FontWeight::Normal}, {"bold", S::FontWeight::Bold}});
        readEnum(obj, "textDecorationStyle", s.textDecorationStyle,
                 {
                     {"solid", S::TextDecorationStyle::Solid},
                     {"double", S::TextDecorationStyle::Double},
                     {"dotted", S::TextDecorationStyle::Dotted},
                     {"dashed", S::TextDecorationStyle::Dashed},
                     {"wavy", S::TextDecorationStyle::Wavy},
                 });
        readEnum(obj, "verticalAlign", s.verticalAlign, verticalAlignValues);
        readEnum(obj, "textAlign", s.textAlign,
                 {
                     {"start", S::TextAlign::Start},
                     {"end", S::TextAlign::End},
                     {"left", S::TextAlign::Left},
                     {"right", S::TextAlign::Right},
                     {"center", S::TextAlign::Center},
                     {"justify", S::TextAlign::Justify},
                     {"justify-all", S::TextAlign::JustifyAll},
                     {"match-parent", S::TextAlign::MatchParent},
                 });
        readEnum(obj, "wordBreak", s.wordBreak,
                 {
                     {"normal", S::WordBreak::Normal},
                     {"break-all", S::WordBreak::BreakAll},
                     {"keep-all", S::WordBreak::KeepAll},
                 });

        if (auto it = obj.find("textDecorationLine"); it != obj.end()) {
            auto lines = std::vector<S::TextDecorationLine>{};

            for (const auto& line : it->is_array() ? *it : nlohmann::json::array({*it})) {
                if (auto value = toEnum(line, textDecorationLineValues)) {
                    lines.push_back(*value);
                }
            }

            s.textDecorationLine = std::move(lines);
        }

        for (auto [key, field] : {
                 std::pair{"fontSize", &s.fontSize},
                 std::pair{"color", &s.color},
                 std::pair{"background", &s.background},
                 std::pair{"backgroundColor", &s.backgroundColor},
                 std::pair{"textDecorationColor", &s.textDecorationColor},
                 std::pair{"borderColor", &s.borderColor},
                 std::pair{"borderStyle", &s.borderStyle},
                 std::pair{"borderRadius", &s.borderRadius},
                 std::pair{"borderWidth", &s.borderWidth},
                 std::pair{"clipPath", &s.clipPath},
                 std::pair{"textEmphasis", &s.textEmphasis},
                 std::pair{"textShadow", &s.textShadow},
                 std::pair{"margin", &s.margin},
                 std::pair{"padding", &s.padding},
                 std::pair{"paddingTop", &s.paddingTop},
                 std::pair{"paddingLeft", &s.paddingLeft},
                 std::pair{"paddingRight", &s.paddingRight},
                 std::pair{"paddingBottom", &s.paddingBottom},
                 std::pair{"whiteSpace", &s.whiteSpace},
                 std::pair{"cursor", &s.cursor},
                 std::pair{"listStyleType", &s.listStyleType},
             }) {
            readString(obj, key, *field);
        }

        // Combines the number (em) and string forms
        for (auto [key, field] : {
                 std::pair{"marginTop", &s.marginTop},
                 std::pair{"marginLeft", &s.marginLeft},
                 std::pair{"marginRight", &s.marginRight},
                 std::pair{"marginBottom", &s.marginBottom},
             }) {
            if (auto it = obj.find(key); it != obj.end()) {
                *field = it->is_number() ? std::format("{}em", it->get<double>())
                                         : it->get<std::string>();
            }
        }
    }

    void from_json(const nlohmann::json& obj, ContentImage& img)
    {
        from_json(obj, static_cast<Image&>(img));

        readEnum(obj, "verticalAlign", img._verticalAlign, verticalAlignValues);
        readString(obj, "border", img._border);
        readString(obj, "borderRadius", img._borderRadius);
        readEnum(obj, "sizeUnits", img._sizeUnits,
                 {{"px", ContentImage::SizeUnits::Px}, {"em", ContentImage::SizeUnits::Em}});
    }

    void from_json(const nlohmann::json& obj, ContentElement& e)
    {
        if (!obj.is_object() || !obj.contains("tag")) {
            throw DictionaryError{"Invalid structured content element: {}", obj.dump()};
        }

        auto tag = obj.at("tag").get<std::string>();
        if (auto value = toEnum(obj.at("tag"), tagValues)) {
            e._tag = *value;
        } else {
            throw DictionaryError{"Unknown structured content tag: {}", tag};
        }

        if (auto it = obj.find("content"); it != obj.end()) {
            e._content = ContentNode::fromJson(*it);
        }

        if (auto it = obj.find("data"); it != obj.end()) {
            it->get_to(e._data);
        }

        if (auto it = obj.find("style"); it != obj.end()) {
            e._style = it->get<ContentStyle>();
        }

        readString(obj, "lang", e._lang);

        switch (e._tag) {
            case ContentTag::Td:
            case ContentTag::Th:
                e._colSpan = obj.contains("colSpan") ? util::toInt(obj.at("colSpan")) : 1;
                e._rowSpan = obj.contains("rowSpan") ? util::toInt(obj.at("rowSpan")) : 1;
                break;

            case ContentTag::Img:
                e._image = obj.get<ContentImage>();
                break;

            case ContentTag::A: {
                if (!obj.contains("href")) {
                    throw DictionaryError{"Link element is missing the required 'href' field: {}",
                                          obj.dump()};
                }

                auto href = obj.at("href").get<std::string>();
                if (!href.starts_with("http:") && !href.starts_with("https:") &&
                    !href.starts_with('?')) {
                    throw DictionaryError{"Invalid link: {}", href};
                }

                e._href = std::move(href);
            } break;

            default:
                readString(obj, "title", e._title);
                e._open = obj.value("open", false);
                break;
        }
    }
} // namespace yomu
