#pragma once

#include "libyomu/Term.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace yomu {
    class Dictionary {
    private:
        struct Metadata {
            std::string title = "";
            std::string description = "";
            std::string revision = "";
        };

        Metadata _meta;
        int _format = 0;
        bool _sequenced = false;

        std::vector<Term> _terms;

    public:
        Dictionary();

        inline const std::vector<Term>& terms() const { return _terms; }

        void loadMetadata(nlohmann::json& obj);

        void loadTermBank(nlohmann::json& arr);

        inline const std::string& title() const { return _meta.title; }
        inline const std::string& revision() const { return _meta.revision; }
        inline int format() const { return _format; }

        static Dictionary* fromFile(const std::string& fileName);
    };
} // namespace yomu
