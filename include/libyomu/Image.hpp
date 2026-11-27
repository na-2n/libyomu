#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <string_view>

namespace yomu {
    // Fields shared by image glossaries and structured content `img` elements
    class Image {
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
        std::optional<double> _width;
        std::optional<double> _height;
        std::optional<std::string> _title;
        std::optional<std::string> _alt;
        RenderingType _rendering = RenderingType::Auto;
        AppearanceType _appearance = AppearanceType::Auto;
        bool _showBackground = true;
        bool _collapsed = false;
        bool _collapsible = true;

    protected:
        Image() = default;

        explicit Image(bool collapsible);

        Image(std::string_view path, bool collapsible);

        // Only used as a base class: protected and non-virtual since images are never
        // deleted through an Image pointer, and copies can only be made of the derived type
        ~Image() = default;
        Image(const Image&) = default;
        Image(Image&&) = default;
        Image& operator=(const Image&) = default;
        Image& operator=(Image&&) = default;

    public:
        [[nodiscard]]
        inline const std::string& path() const
        {
            return _path;
        }

        // Preferred display size
        [[nodiscard]]
        inline const std::optional<double>& width() const
        {
            return _width;
        }

        [[nodiscard]]
        inline const std::optional<double>& height() const
        {
            return _height;
        }

        // Hover text
        [[nodiscard]]
        inline const std::optional<std::string>& title() const
        {
            return _title;
        }

        [[nodiscard]]
        inline const std::optional<std::string>& alt() const
        {
            return _alt;
        }

        [[nodiscard]]
        inline RenderingType rendering() const
        {
            return _rendering;
        }

        [[nodiscard]]
        inline AppearanceType appearance() const
        {
            return _appearance;
        }

        [[nodiscard]]
        inline bool collapsed() const
        {
            return _collapsed;
        }

        [[nodiscard]]
        inline bool collapsible() const
        {
            return _collapsible;
        }

        [[nodiscard]]
        inline bool showBackground() const
        {
            return _showBackground;
        }

        friend void from_json(const nlohmann::json& j, Image& img);
    };

    void from_json(const nlohmann::json& j, Image& img);
} // namespace yomu
