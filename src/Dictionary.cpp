#include "libyomu/Dictionary.hpp"

#include "libyomu/Term.hpp"

#include <libzippp.h>
#include <nlohmann/json.hpp>

#include <format>
#include <iostream>
#include <ranges>
#include <string>

#define LOG_INFO(str, ...) (std::cout << std::format(str, __VA_ARGS__) << std::endl)

namespace yomu {
    void Dictionary::loadMetadata(nlohmann::json& obj)
    {
        if (!obj.contains("title")) {
            throw; // TODO: throw useful error
        }

        if (!obj.contains("revision")) {
            throw; // TODO: throw useful error;
        }

        if (obj.contains("format")) {
            _format = obj.at("format");
        } else if (obj.contains("version")) {
            _format = obj.at("version");
        } else {
            throw; // TODO: throw useful error:
        }

        _meta.title = obj.at("title");
        _meta.revision = obj.at("revision");

        if (obj.contains("sequenced")) {
            _sequenced = obj.at("sequenced");
        }

        if (obj.contains("description")) {
            _meta.description = obj.at("description");
        }
    }

    void Dictionary::loadTermBank(nlohmann::json& arr)
    {
        if (!arr.is_array()) {
            throw; // TODO: return useful error
        }

        for (auto obj : arr) {
            _terms.push_back(Term::fromJson(obj, _format));
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
            Unknown,
        };

        auto zip = libzippp::ZipArchive{fileName};

        zip.open();
        if (!zip.isOpen()) {
            throw; // TODO: throw useful error
        }

        auto dict = std::make_unique<Dictionary>();
        auto loadEntry = [&](libzippp::ZipEntry& entry, FileType type) {
            auto data = entry.readAsBinary();
            auto obj = nlohmann::json::parse(data, data + entry.getSize());

            LOG_INFO("Loading file: {}", entry.getName());

            switch (type) {
                case FileType::Index:
                    dict->loadMetadata(obj);
                    break;

                case FileType::TermBank:
                    dict->loadTermBank(obj);

                default:
                    break;
            }
        };

        auto index = zip.getEntry("index.json");
        if (index.isNull()) {
            throw; // TODO: throw useful error
        }

        loadEntry(index, FileType::Index);

        auto entries = zip.getEntries() | std::views::filter([](auto& e) {
                           return e.isFile() && e.getName() != "index.json";
                       });
        for (auto& entry : entries) {
            auto name = entry.getName();
            auto type = FileType::Unknown;

            if (name.starts_with(Term::FileMatch)) {
                type = FileType::TermBank;
            }

            if (type == FileType::Unknown) {
                continue;
            }

            loadEntry(entry, type);
        }

        return dict;
    }
} // namespace yomu
