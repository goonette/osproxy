#include "modules.hpp"

#include <windows.h>
#include <winternl.h>

auto modules::list() -> std::vector<module_entry> {
    auto result = std::vector<module_entry>{};
    const auto peb = NtCurrentTeb()->ProcessEnvironmentBlock;

    if (!peb || !peb->Ldr)
        throw std::runtime_error{"failed to enumerate module list"};

    const auto list = &peb->Ldr->InMemoryOrderModuleList;

    for (auto entry = list->Flink; entry != list; entry = entry->Flink) {
        const auto module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (!module || !module->DllBase || !module->FullDllName.Buffer)
            continue;

        const auto base = static_cast<std::uint8_t*>(module->DllBase);
        const auto nt = nt_headers(base);

        if (!nt || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
            continue;

        result.emplace_back(module_entry{
            .base = base,
            .size = nt->OptionalHeader.SizeOfImage,
            .path = std::wstring{module->FullDllName.Buffer,
                                 module->FullDllName.Length / sizeof(wchar_t)},
        });
    }

    return result;
}

auto modules::nt_headers(std::uint8_t* module) -> nt_headers_type {
    const auto dos = reinterpret_cast<dos_header_type>(module);

    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return nullptr;

    const auto nt = reinterpret_cast<nt_headers_type>(module + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return nullptr;

    return nt;
}

auto modules::has_export(std::uint8_t* module, std::string_view symbol) -> bool {
    const auto nt = nt_headers(module);

    if (!nt || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
        return false;

    const auto [exports_va, exports_size] =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    const auto exports = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(module + exports_va);

    if (!exports_va || exports->NumberOfNames == 0)
        return false;

    const auto names = reinterpret_cast<DWORD*>(module + exports->AddressOfNames);

    for (auto i = 0ul; i < exports->NumberOfNames; ++i) {
        if (symbol == reinterpret_cast<const char*>(module + names[i]))
            return true;
    }

    return false;
}