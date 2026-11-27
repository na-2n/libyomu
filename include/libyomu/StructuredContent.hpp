#pragma once

#include "libyomu/Image.hpp"

#include <nlohmann/json.hpp>

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace yomu {
    struct ContentStyle {
        enum class FontStyle {
            Normal,
            Italic,
        };

        enum class FontWeight {
            Normal,
            Bold,
        };

        enum class TextDecorationLine {
            Underline,
            Overline,
            LineThrough,
        };

        enum class TextDecorationStyle {
            Solid,
            Double,
            Dotted,
            Dashed,
            Wavy,
        };

        enum class VerticalAlign {
            Baseline,
            Sub,
            Super,
            TextTop,
            TextBottom,
            Middle,
            Top,
            Bottom,
        };

        enum class TextAlign {
            Start,
            End,
            Left,
            Right,
            Center,
            Justify,
            JustifyAll,
            MatchParent,
        };

        enum class WordBreak {
            Normal,
            BreakAll,
            KeepAll,
        };

        std::optional<FontStyle> fontStyle;
        std::optional<FontWeight> fontWeight;
        std::optional<std::string> fontSize;
        std::optional<std::string> color;
        std::optional<std::string> background;
        std::optional<std::string> backgroundColor;
        // Combines the single value and array forms, "none" is an empty list
        std::optional<std::vector<TextDecorationLine>> textDecorationLine;
        std::optional<TextDecorationStyle> textDecorationStyle;
        std::optional<std::string> textDecorationColor;
        std::optional<std::string> borderColor;
        std::optional<std::string> borderStyle;
        std::optional<std::string> borderRadius;
        std::optional<std::string> borderWidth;
        std::optional<std::string> clipPath;
        std::optional<VerticalAlign> verticalAlign;
        std::optional<TextAlign> textAlign;
        std::optional<std::string> textEmphasis;
        std::optional<std::string> textShadow;
        std::optional<std::string> margin;
        // Numbers are converted to em values, e.g. 0.5 -> "0.5em"
        std::optional<std::string> marginTop;
        std::optional<std::string> marginLeft;
        std::optional<std::string> marginRight;
        std::optional<std::string> marginBottom;
        std::optional<std::string> padding;
        std::optional<std::string> paddingTop;
        std::optional<std::string> paddingLeft;
        std::optional<std::string> paddingRight;
        std::optional<std::string> paddingBottom;
        std::optional<WordBreak> wordBreak;
        std::optional<std::string> whiteSpace;
        std::optional<std::string> cursor;
        std::optional<std::string> listStyleType;
    };

    class ContentImage : public Image {
    public:
        enum class SizeUnits {
            Px,
            Em,
        };

    private:
        std::optional<ContentStyle::VerticalAlign> _verticalAlign;
        std::optional<std::string> _border;
        std::optional<std::string> _borderRadius;
        std::optional<SizeUnits> _sizeUnits;

    public:
        // Unlike image glossaries, `img` elements are not collapsible by default
        ContentImage()
            : Image{false}
        {}

        [[nodiscard]]
        inline const std::optional<ContentStyle::VerticalAlign>& verticalAlign() const
        {
            return _verticalAlign;
        }

        [[nodiscard]]
        inline const std::optional<std::string>& border() const
        {
            return _border;
        }

        [[nodiscard]]
        inline const std::optional<std::string>& borderRadius() const
        {
            return _borderRadius;
        }

        [[nodiscard]]
        inline const std::optional<SizeUnits>& sizeUnits() const
        {
            return _sizeUnits;
        }

        friend void from_json(const nlohmann::json& j, ContentImage& img);
    };

    enum class ContentTag {
        Br,
        Ruby,
        Rt,
        Rp,
        Table,
        Thead,
        Tbody,
        Tfoot,
        Tr,
        Td,
        Th,
        Span,
        Div,
        Ol,
        Ul,
        Li,
        Details,
        Summary,
        Img,
        A,
    };

    class ContentNode;

    class ContentElement {
    private:
        ContentTag _tag = ContentTag::Span;
        std::vector<ContentNode> _content;
        std::map<std::string, std::string> _data;
        std::optional<std::string> _lang;
        std::optional<ContentStyle> _style;
        std::optional<std::string> _title;
        bool _open = false;
        int _colSpan = 1;
        int _rowSpan = 1;
        std::optional<std::string> _href;
        std::optional<ContentImage> _image;

    public:
        ContentElement() = default;

        [[nodiscard]]
        inline ContentTag tag() const
        {
            return _tag;
        }

        // Child nodes, nested arrays are flattened
        [[nodiscard]]
        inline const std::vector<ContentNode>& content() const
        {
            return _content;
        }

        // Rendered as `data-*` attributes
        [[nodiscard]]
        inline const std::map<std::string, std::string>& data() const
        {
            return _data;
        }

        [[nodiscard]]
        inline const std::optional<std::string>& lang() const
        {
            return _lang;
        }

        [[nodiscard]]
        inline const std::optional<ContentStyle>& style() const
        {
            return _style;
        }

        // Hover text
        [[nodiscard]]
        inline const std::optional<std::string>& title() const
        {
            return _title;
        }

        // Only used by `details`
        [[nodiscard]]
        inline bool open() const
        {
            return _open;
        }

        // Only used by `td` and `th`
        [[nodiscard]]
        inline int colSpan() const
        {
            return _colSpan;
        }

        [[nodiscard]]
        inline int rowSpan() const
        {
            return _rowSpan;
        }

        // Only set for `a`
        [[nodiscard]]
        inline const std::optional<std::string>& href() const
        {
            return _href;
        }

        // Links starting with '?' are searches within the dictionary
        [[nodiscard]]
        inline bool internalLink() const
        {
            return _href && _href->starts_with('?');
        }

        // Only set for `img`
        [[nodiscard]]
        inline const std::optional<ContentImage>& image() const
        {
            return _image;
        }

        friend void from_json(const nlohmann::json& j, ContentElement& e);
    };

    // A text node or an element
    class ContentNode {
    private:
        std::variant<std::string, ContentElement> _value;

    public:
        ContentNode() = default;

        explicit ContentNode(std::string text);

        explicit ContentNode(ContentElement element);

        [[nodiscard]]
        inline const std::string* text() const noexcept
        {
            return std::get_if<std::string>(&_value);
        }

        [[nodiscard]]
        inline const ContentElement* element() const noexcept
        {
            return std::get_if<ContentElement>(&_value);
        }

        // Parses a string, element or (nested) array of content
        static std::vector<ContentNode> fromJson(const nlohmann::json& obj);
    };

    void from_json(const nlohmann::json& j, ContentStyle& s);
    void from_json(const nlohmann::json& j, ContentImage& img);
    void from_json(const nlohmann::json& j, ContentElement& e);
} // namespace yomu
