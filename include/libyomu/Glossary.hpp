#pragma once

#include <nlohmann/json.hpp>

#include <memory>
#include <optional>
#include <string>

namespace yomu {
    enum class GlossaryType {
        Text,
        Image,
        Structured,
        Deinflection,
        Unknown,
    };

    class GlossaryItem {
    public:
        virtual ~GlossaryItem() = default;
    };

    class TextGlossary : public GlossaryItem {
    private:
        std::string _text;

    public:
        TextGlossary() = default;
        TextGlossary(std::string_view text);

        [[nodiscard]]
        inline const std::string& text() const { return _text; }

        friend void from_json(const nlohmann::json& j, TextGlossary& g);
    };

    class ImageGlossary : public GlossaryItem {
    public:
        enum class RenderingType {
            Auto,
            Pixelated,
            CrispEdges,
        };

        enum class AppearanceType {
            Auto,
            Monochrome,
        };

    private:
        std::string _path;
        std::optional<int> _width;
        std::optional<int> _height;
        std::optional<std::string> _title;
        std::optional<std::string> _alt;
        std::optional<std::string> _description;
        RenderingType _rendering = RenderingType::Auto;
        AppearanceType _appearance = AppearanceType::Auto;
        bool _showBackground = true;
        bool _collapsed = false;
        bool _collapsible = true;

    public:
        ImageGlossary() = default;

        ImageGlossary(std::string_view path);

        [[nodiscard]]
        inline const std::string& path() const { return _path; }

        // Preferred display size
        [[nodiscard]]
        inline const std::optional<int>& width() const { return _width; }

        [[nodiscard]]
        inline const std::optional<int>& height() const { return _height; }

        // Hover text
        [[nodiscard]]
        inline const std::optional<std::string>& title() const { return _title; }

        [[nodiscard]]
        inline const std::optional<std::string>& alt() const { return _alt; }

        [[nodiscard]]
        inline const std::optional<std::string>& description() const { return _description; }

        [[nodiscard]]
        inline RenderingType rendering() const { return _rendering; }

        [[nodiscard]]
        inline AppearanceType appearance() const { return _appearance; }

        [[nodiscard]]
        inline bool collapsed() const { return _collapsed; }

        [[nodiscard]]
        inline bool collapsible() const { return _collapsible; }

        [[nodiscard]]
        inline bool showBackground() const { return _showBackground; }

        friend void from_json(const nlohmann::json& j, ImageGlossary& g);
    };

    class Glossary {
    private:
        GlossaryType _type = GlossaryType::Unknown;
        std::unique_ptr<GlossaryItem> _item = nullptr;

    public:
        Glossary() = default;

        Glossary(GlossaryType type, std::unique_ptr<GlossaryItem> item);

        [[nodiscard]]
        inline GlossaryType type() const noexcept { return _type; }

        [[nodiscard]]
        inline ImageGlossary* image() const noexcept
        {
            return dynamic_cast<ImageGlossary*>(_item.get());
        }

        [[nodiscard]]
        inline TextGlossary* text() const noexcept
        {
            return dynamic_cast<TextGlossary*>(_item.get());
        }

        friend void from_json(const nlohmann::json& j, Glossary& g);
    };

    void from_json(const nlohmann::json& j, TextGlossary& g);
    void from_json(const nlohmann::json& j, ImageGlossary& g);
    void from_json(const nlohmann::json& j, Glossary& g);
} // namespace yomu
