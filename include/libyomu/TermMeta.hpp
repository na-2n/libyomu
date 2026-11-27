#pragma once

#include "libyomu/Frequency.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace yomu {
    enum class TermMetaType {
        Frequency,
        Pitch,
        IPA,
        Unknown,
    };

    class TermMetaItem {
    public:
        virtual ~TermMetaItem() = default;
    };

    class FreqTermMeta : public TermMetaItem {
    private:
        std::optional<std::string> _reading;
        Frequency _frequency;

    public:
        FreqTermMeta() = default;

        // Empty when the frequency applies to every reading of the expression
        [[nodiscard]]
        inline const std::optional<std::string>& reading() const { return _reading; }

        [[nodiscard]]
        inline const Frequency& frequency() const { return _frequency; }

        friend void from_json(const nlohmann::json& j, FreqTermMeta& m);
    };

    class PitchTermMeta : public TermMetaItem {
    public:
        class Pitch {
        private:
            // Downstep mora position, or a per-mora "HL" pattern
            std::variant<int, std::string> _position = 0;
            std::vector<int> _nasal;
            std::vector<int> _devoice;
            std::vector<std::string> _tags;

            friend void from_json(const nlohmann::json& j, Pitch& p);

        public:
            Pitch() = default;

            [[nodiscard]]
            inline const std::variant<int, std::string>& position() const { return _position; }

            [[nodiscard]]
            inline std::optional<int> downstep() const
            {
                if (const auto* pos = std::get_if<int>(&_position)) {
                    return *pos;
                }
                return std::nullopt;
            }

            [[nodiscard]]
            inline std::optional<std::string> pattern() const
            {
                if (const auto* pos = std::get_if<std::string>(&_position)) {
                    return *pos;
                }
                return std::nullopt;
            }

            [[nodiscard]]
            inline const std::vector<int>& nasalPositions() const { return _nasal; }

            [[nodiscard]]
            inline const std::vector<int>& devoicePositions() const { return _devoice; }

            [[nodiscard]]
            inline const std::vector<std::string>& tags() const { return _tags; }
        };

    private:
        std::string _reading;
        std::vector<Pitch> _pitches;

    public:
        PitchTermMeta() = default;

        [[nodiscard]]
        inline const std::string& reading() const { return _reading; }

        [[nodiscard]]
        inline const std::vector<Pitch>& pitches() const { return _pitches; }

        friend void from_json(const nlohmann::json& j, PitchTermMeta& m);
    };

    class IpaTermMeta : public TermMetaItem {
    public:
        struct Transcription {
            std::string ipa;
            std::vector<std::string> tags;
        };

    private:
        std::string _reading;
        std::vector<Transcription> _transcriptions;

    public:
        IpaTermMeta() = default;

        [[nodiscard]]
        inline const std::string& reading() const { return _reading; }

        [[nodiscard]]
        inline const std::vector<Transcription>& transcriptions() const { return _transcriptions; }

        friend void from_json(const nlohmann::json& j, IpaTermMeta& m);
    };

    class TermMeta {
    private:
        std::string _expression;
        TermMetaType _type = TermMetaType::Unknown;
        std::unique_ptr<TermMetaItem> _item = nullptr;

    public:
        TermMeta() = default;

        inline static const std::string FileMatch = "term_meta_bank_";

        [[nodiscard]]
        inline const std::string& expression() const { return _expression; }

        [[nodiscard]]
        inline TermMetaType type() const noexcept { return _type; }

        [[nodiscard]]
        inline FreqTermMeta* frequency() const noexcept
        {
            return dynamic_cast<FreqTermMeta*>(_item.get());
        }

        [[nodiscard]]
        inline PitchTermMeta* pitch() const noexcept
        {
            return dynamic_cast<PitchTermMeta*>(_item.get());
        }

        [[nodiscard]]
        inline IpaTermMeta* ipa() const noexcept
        {
            return dynamic_cast<IpaTermMeta*>(_item.get());
        }

        static TermMeta fromJson(const nlohmann::json& arr);
    };

    void from_json(const nlohmann::json& j, FreqTermMeta& m);
    void from_json(const nlohmann::json& j, PitchTermMeta::Pitch& p);
    void from_json(const nlohmann::json& j, PitchTermMeta& m);
    void from_json(const nlohmann::json& j, IpaTermMeta::Transcription& t);
    void from_json(const nlohmann::json& j, IpaTermMeta& m);
} // namespace yomu
