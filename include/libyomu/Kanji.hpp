#pragma once

#include <vector>
#include <string>

namespace yomu {
    class Kanji {
    public:
        const std::string& character() const;

        const std::string& onyomi() const;

        const std::string& kunyomi() const;

        const std::vector<std::string>& tags() const;

        const std::vector<std::string>& meanings() const;
    };
}

