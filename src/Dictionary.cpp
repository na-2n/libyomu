#include "libyomu/Dictionary.hpp"

#include "libyomu/Error.hpp"
#include "libyomu/Kanji.hpp"
#include "libyomu/Tag.hpp"
#include "libyomu/Term.hpp"
#include "libyomu/TermMeta.hpp"

#include <libzippp.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <format>
#include <iostream>
#include <ranges>
#include <string>

#ifdef _DEBUG
#define LOG_DBG(str, ...) (std::cout << std::format(str, __VA_ARGS__) << std::endl)
#else
#define LOG_DBG(str, ...)
#endif

namespace yomu {
    namespace {
        // Matches Yomitan's /^<prefix>(\d+)\.json$/
        bool isBankFile(std::string_view name, std::string_view prefix)
        {
            constexpr auto ext = std::string_view{".json"};

            if (!name.starts_with(prefix) || !name.ends_with(ext)) {
                return false;
            }

            auto num = name.substr(prefix.size(), name.size() - prefix.size() - ext.size());
            return !num.empty() && std::ranges::all_of(num, [](char c) { return c >= '0' && c <= '9'; });
        }

        bool isHttpUrl(std::string_view url)
        {
            return url.starts_with("http://") || url.starts_with("https://");
        }

        void requireArray(const nlohmann::json& arr, std::string_view bank)
        {
            if (!arr.is_array()) {
                throw DictionaryError{"{} must be an array, got {}", bank, arr.type_name()};
            }
        }
    } // namespace

    void Dictionary::loadMetadata(const nlohmann::json& obj)
    {
        if (!obj.is_object()) {
            throw DictionaryError{"index.json must be an object, got {}", obj.type_name()};
        }

        if (!obj.contains("title")) {
            throw DictionaryError{"index.json is missing the required 'title' field"};
        }

        if (!obj.contains("revision")) {
            throw DictionaryError{"index.json is missing the required 'revision' field"};
        }

        if (obj.contains("format") && obj.at("format").is_number()) {
            _format = obj.at("format");
        } else if (obj.contains("version")) {
            _format = obj.at("version");
        } else {
            throw DictionaryError{"index.json is missing the required 'format' or 'version' field"};
        }

        if (_format < 1 || _format > 3) {
            throw DictionaryError{"Unsupported dictionary format: {}", _format};
        }

        _meta.title = obj.at("title");
        _meta.revision = obj.at("revision");

        if (obj.contains("sequenced")) {
            _sequenced = obj.at("sequenced");
        }

        for (auto [key, field] : {
                 std::pair{"description", &_meta.description},
                 std::pair{"author", &_meta.author},
                 std::pair{"url", &_meta.url},
                 std::pair{"attribution", &_meta.attribution},
                 std::pair{"sourceLanguage", &_meta.sourceLanguage},
                 std::pair{"targetLanguage", &_meta.targetLanguage},
                 std::pair{"minimumYomitanVersion", &_meta.minimumYomitanVersion},
             }) {
            if (obj.contains(key)) {
                *field = obj.at(key).get<std::string>();
            }
        }

        if (obj.contains("frequencyMode")) {
            auto mode = obj.at("frequencyMode").get<std::string>();

            if (mode == "occurrence-based") {
                _meta.frequencyMode = FrequencyMode::OccurrenceBased;
            } else if (mode == "rank-based") {
                _meta.frequencyMode = FrequencyMode::RankBased;
            } else {
                throw DictionaryError{"Unknown frequency mode: {}", mode};
            }
        }

        // Yomitan rejects `isUpdatable: false` and non-http(s) update URLs
        if (obj.contains("isUpdatable")) {
            auto indexUrl = obj.value("indexUrl", "");
            auto downloadUrl = obj.value("downloadUrl", "");

            if (!obj.at("isUpdatable").get<bool>() || !isHttpUrl(indexUrl) || !isHttpUrl(downloadUrl)) {
                throw DictionaryError{"Invalid index data for updatable dictionary"};
            }

            _meta.update = UpdateInfo{
                .indexUrl = std::move(indexUrl),
                .downloadUrl = std::move(downloadUrl),
            };
        }

        // Legacy tag definitions, stored alongside the tag banks
        if (obj.contains("tagMeta")) {
            for (auto& [name, tag] : obj.at("tagMeta").items()) {
                _tags.push_back(Tag::fromJson(name, tag));
            }
        }
    }

    void Dictionary::loadTermBank(const nlohmann::json& arr)
    {
        requireArray(arr, "Term bank");

        for (const auto& obj : arr) {
            _terms.push_back(Term::fromJson(obj, _format));
        }
    }

    void Dictionary::loadTermMetaBank(const nlohmann::json& arr)
    {
        requireArray(arr, "Term meta bank");

        for (const auto& obj : arr) {
            _termMeta.push_back(TermMeta::fromJson(obj));
        }
    }

    void Dictionary::loadKanjiBank(const nlohmann::json& arr)
    {
        requireArray(arr, "Kanji bank");

        for (const auto& obj : arr) {
            _kanji.push_back(Kanji::fromJson(obj, _format));
        }
    }

    void Dictionary::loadKanjiMetaBank(const nlohmann::json& arr)
    {
        requireArray(arr, "Kanji meta bank");

        for (const auto& obj : arr) {
            _kanjiMeta.push_back(KanjiMeta::fromJson(obj));
        }
    }

    void Dictionary::loadTagBank(const nlohmann::json& arr)
    {
        requireArray(arr, "Tag bank");

        for (const auto& obj : arr) {
            _tags.push_back(Tag::fromJson(obj));
        }
    }

    std::unique_ptr<Dictionary> Dictionary::fromFile(const std::string& fileName)
    {
        enum class FileType {
            Index,
            TermBank,
            TermMetaBank,
            KanjiBank,
            KanjiMetaBank,
            TagBank,
            Unknown,
        };

        auto zip = libzippp::ZipArchive{fileName};

        zip.open();
        if (!zip.isOpen()) {
            throw DictionaryError{"Failed to open dictionary archive: {}", fileName};
        }

        auto dict = std::make_unique<Dictionary>();
        auto loadEntry = [&](libzippp::ZipEntry& entry, FileType type) {
            // readAsBinary hands over ownership of the buffer
            auto data = std::unique_ptr<libzippp_uint8[]>{entry.readAsBinary()};
            if (data == nullptr) {
                throw DictionaryError{"Failed to read {} from {}", entry.getName(), fileName};
            }

            auto obj = nlohmann::json::parse(data.get(), data.get() + entry.getSize()); // NOLINT(*-pointer-arithmetic)

            LOG_DBG("Loading file: {}", entry.getName());

            switch (type) {
                case FileType::Index:
                    dict->loadMetadata(obj);
                    break;

                case FileType::TermBank:
                    dict->loadTermBank(obj);
                    break;

                case FileType::TermMetaBank:
                    dict->loadTermMetaBank(obj);
                    break;

                case FileType::KanjiBank:
                    dict->loadKanjiBank(obj);
                    break;

                case FileType::KanjiMetaBank:
                    dict->loadKanjiMetaBank(obj);
                    break;

                case FileType::TagBank:
                    dict->loadTagBank(obj);
                    break;

                default:
                    break;
            }
        };

        auto index = zip.getEntry("index.json");
        if (index.isNull()) {
            throw DictionaryError{"Dictionary archive {} has no index.json in its root", fileName};
        }

        loadEntry(index, FileType::Index);

        auto entries = zip.getEntries() | std::views::filter([](auto& e) {
                           return e.isFile() && e.getName() != "index.json";
                       });
        for (auto& entry : entries) {
            auto name = entry.getName();

            auto type = isBankFile(name, Term::FileMatch)        ? FileType::TermBank
                        : isBankFile(name, TermMeta::FileMatch)  ? FileType::TermMetaBank
                        : isBankFile(name, Kanji::FileMatch)     ? FileType::KanjiBank
                        : isBankFile(name, KanjiMeta::FileMatch) ? FileType::KanjiMetaBank
                        : isBankFile(name, Tag::FileMatch)       ? FileType::TagBank
                                                                 : FileType::Unknown;

            if (type == FileType::Unknown) {
                continue;
            }

            loadEntry(entry, type);
        }

        return dict;
    }
} // namespace yomu
