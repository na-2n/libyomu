#pragma once

#include "libyomu/Image.hpp"
#include "libyomu/StructuredContent.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

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

    protected:
        // Protected so items can only be copied or moved as their concrete type,
        // preventing slicing through a GlossaryItem reference
        GlossaryItem() = default;
        GlossaryItem(const GlossaryItem&) = default;
        GlossaryItem(GlossaryItem&&) = default;
        GlossaryItem& operator=(const GlossaryItem&) = default;
        GlossaryItem& operator=(GlossaryItem&&) = default;
    };

    class TextGlossary : public GlossaryItem {
    private:
        std::string _text;

    public:
        TextGlossary() = default;
        TextGlossary(std::string_view text)
            : _text{text}
        {}

        [[nodiscard]]
        inline const std::string& text() const
        {
            return _text;
        }

        friend void from_json(const nlohmann::json& j, TextGlossary& g);
    };

    class ImageGlossary : public GlossaryItem, public Image {
    private:
        std::optional<std::string> _description;

    public:
        ImageGlossary()
            : Image{true}
        {}

        ImageGlossary(std::string_view path)
            : Image{path, true}
        {}

        [[nodiscard]]
        inline const std::optional<std::string>& description() const
        {
            return _description;
        }

        friend void from_json(const nlohmann::json& j, ImageGlossary& g);
    };

    class StructuredGlossary : public GlossaryItem {
    private:
        std::vector<ContentNode> _content;

    public:
        StructuredGlossary() = default;

        // Root content nodes, nested arrays are flattened
        [[nodiscard]]
        inline const std::vector<ContentNode>& content() const
        {
            return _content;
        }

        friend void from_json(const nlohmann::json& j, StructuredGlossary& g);
    };

    // Marks the term as an inflected form of another term
    class DeinflectionGlossary : public GlossaryItem {
    private:
        std::string _uninflected;
        std::vector<std::string> _rules;

    public:
        DeinflectionGlossary() = default;

        [[nodiscard]]
        inline const std::string& uninflected() const
        {
            return _uninflected;
        }

        // Chain of inflection rules leading from the uninflected term to this one
        [[nodiscard]]
        inline const std::vector<std::string>& rules() const
        {
            return _rules;
        }

        friend void from_json(const nlohmann::json& j, DeinflectionGlossary& g);
    };

    class Glossary {
    private:
        GlossaryType _type = GlossaryType::Unknown;
        std::unique_ptr<GlossaryItem> _item = nullptr;

    public:
        Glossary() = default;

        Glossary(GlossaryType type, std::unique_ptr<GlossaryItem> item)
            : _type{type}, _item{std::move(item)}
        {}

        [[nodiscard]]
        inline GlossaryType type() const noexcept
        {
            return _type;
        }

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

        [[nodiscard]]
        inline StructuredGlossary* structured() const noexcept
        {
            return dynamic_cast<StructuredGlossary*>(_item.get());
        }

        [[nodiscard]]
        inline DeinflectionGlossary* deinflection() const noexcept
        {
            return dynamic_cast<DeinflectionGlossary*>(_item.get());
        }

        friend void from_json(const nlohmann::json& j, Glossary& g);
    };

    void from_json(const nlohmann::json& j, TextGlossary& g);
    void from_json(const nlohmann::json& j, ImageGlossary& g);
    void from_json(const nlohmann::json& j, StructuredGlossary& g);
    void from_json(const nlohmann::json& j, DeinflectionGlossary& g);
    void from_json(const nlohmann::json& j, Glossary& g);
} // namespace yomu
