#pragma once

#include <string>

namespace yomu {
    class Tag {
    public:
        const std::string& name() const;

        const std::string& category() const;

        int order() const;

        const std::string& notes() const;

        int score() const;
    };
}

