#pragma once

#include <nlohmann/json.hpp>

#include <memory>

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
        TextGlossary(std::string_view text);

        inline const std::string& text() const { return _text; }
    };

    class ImageGlossary : public GlossaryItem {
    private:

    public:
        static ImageGlossary fromJson();
    };

    class Glossary {
    private:
        GlossaryType _type;
        std::unique_ptr<GlossaryItem> _item;

        Glossary(GlossaryType type, std::unique_ptr<GlossaryItem> item);

    public:
        inline GlossaryType type() const noexcept { return _type; }

        inline ImageGlossary* image() const noexcept
        {
            return dynamic_cast<ImageGlossary*>(_item.get());
        }

        inline TextGlossary* text() const noexcept
        {
            return dynamic_cast<TextGlossary*>(_item.get());
        }

        static Glossary fromJson(const nlohmann::json& o);
    };
} // namespace yomu
