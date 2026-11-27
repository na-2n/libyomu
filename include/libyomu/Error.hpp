#pragma once

#include <format>
#include <stdexcept>
#include <utility>

namespace yomu {
    class DictionaryError : public std::runtime_error {
    public:
        template<typename... Args>
        explicit DictionaryError(std::format_string<Args...> fmt, Args&&... args)
            : std::runtime_error{std::format(fmt, std::forward<Args>(args)...)}
        {}
    };
} // namespace yomu
