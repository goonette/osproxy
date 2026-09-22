#pragma once

#include <fmt/format.h>

namespace console {

namespace log {

template <typename... Args>
auto constexpr warn(fmt::format_string<Args...> fmt, Args&&... args) {
    std::printf("[warn] %s\n", fmt::format(fmt, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
auto constexpr info(fmt::format_string<Args...> fmt, Args&&... args) {
    std::printf("[info] %s\n", fmt::format(fmt, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
auto constexpr misc(fmt::format_string<Args...> fmt, Args&&... args) {
    std::printf("[misc] %s\n", fmt::format(fmt, std::forward<Args>(args)...).c_str());
}

} // namespace log

} // namespace console