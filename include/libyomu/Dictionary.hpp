#pragma once

#include "libyomu/Kanji.hpp"
#include "libyomu/Tag.hpp"
#include "libyomu/Term.hpp"
#include "libyomu/TermMeta.hpp"

#include <nlohmann/json.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace yomu {
    class Dictionary {
    public:
        enum class FrequencyMode {
            OccurrenceBased, // higher value is more common
            RankBased, // lower value is more common
        };

        // Combines the `isUpdatable`, `indexUrl` and `downloadUrl` fields
        struct UpdateInfo {
            std::string indexUrl;
            std::string downloadUrl;
        };

    private:
        struct Metadata {
            std::string title = "";
            std::string revision = "";
            std::optional<std::string> description;
            std::optional<std::string> author;
            std::optional<std::string> url;
            std::optional<std::string> attribution;
            std::optional<std::string> sourceLanguage;
            std::optional<std::string> targetLanguage;
            std::optional<std::string> minimumYomitanVersion;
            std::optional<FrequencyMode> frequencyMode;
            std::optional<UpdateInfo> update;
        };

        Metadata _meta;
        int _format = 0;
        bool _sequenced = false;

        std::vector<Term> _terms;
        std::vector<TermMeta> _termMeta;
        std::vector<Kanji> _kanji;
        std::vector<KanjiMeta> _kanjiMeta;
        std::vector<Tag> _tags;

    public:
        [[nodiscard]]
        inline const std::vector<Term>& terms() const { return _terms; }

        [[nodiscard]]
        inline const std::vector<TermMeta>& termMeta() const { return _termMeta; }

        [[nodiscard]]
        inline const std::vector<Kanji>& kanji() const { return _kanji; }

        [[nodiscard]]
        inline const std::vector<KanjiMeta>& kanjiMeta() const { return _kanjiMeta; }

        // Tags from tag banks and the legacy `tagMeta` index field
        [[nodiscard]]
        inline const std::vector<Tag>& tags() const { return _tags; }

        void loadMetadata(const nlohmann::json& obj);

        void loadTermBank(const nlohmann::json& arr);

        void loadTermMetaBank(const nlohmann::json& arr);

        void loadKanjiBank(const nlohmann::json& arr);

        void loadKanjiMetaBank(const nlohmann::json& arr);

        void loadTagBank(const nlohmann::json& arr);

        [[nodiscard]]
        inline const std::string& title() const { return _meta.title; }

        [[nodiscard]]
        inline const std::optional<std::string>& description() const { return _meta.description; }

        [[nodiscard]]
        inline const std::string& revision() const { return _meta.revision; }

        [[nodiscard]]
        inline const std::optional<std::string>& author() const { return _meta.author; }

        [[nodiscard]]
        inline const std::optional<std::string>& url() const { return _meta.url; }

        [[nodiscard]]
        inline const std::optional<std::string>& attribution() const { return _meta.attribution; }

        [[nodiscard]]
        inline const std::optional<std::string>& sourceLanguage() const { return _meta.sourceLanguage; }

        [[nodiscard]]
        inline const std::optional<std::string>& targetLanguage() const { return _meta.targetLanguage; }

        [[nodiscard]]
        inline const std::optional<std::string>& minimumYomitanVersion() const { return _meta.minimumYomitanVersion; }

        [[nodiscard]]
        inline const std::optional<FrequencyMode>& frequencyMode() const { return _meta.frequencyMode; }

        [[nodiscard]]
        inline const std::optional<UpdateInfo>& update() const { return _meta.update; }

        [[nodiscard]]
        inline int format() const { return _format; }

        [[nodiscard]]
        inline bool sequenced() const { return _sequenced; }

        static std::unique_ptr<Dictionary> fromFile(const std::string& fileName);
    };
} // namespace yomu
