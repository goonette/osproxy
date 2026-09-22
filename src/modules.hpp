#pragma once

#include <windows.h>

#include <span>
#include <vector>
#include <filesystem>

namespace modules {

using dos_header_type = PIMAGE_DOS_HEADER;
using nt_headers_type = PIMAGE_NT_HEADERS;

struct module_entry {
    std::uint8_t* base;
    std::size_t size;
    std::filesystem::path path;

    [[nodiscard]] auto region() const -> std::span<std::uint8_t> { return {base, size}; }
};

auto list() -> std::vector<module_entry>;
auto nt_headers(std::uint8_t* module) -> nt_headers_type;
auto has_export(std::uint8_t* module, std::string_view symbol) -> bool;

} // namespace modules