#include <safetyhook.hpp>

#include "osrs.hpp"
#include "console.hpp"
#include "modules.hpp"

#include <fmt/ranges.h>

using namespace osrs;
using namespace console;

static auto hk_send_client_message = safetyhook::InlineHook{};

// credit: https://github.com/blurite/rsprox
static auto read_u16_alt3(std::uint16_t value) -> std::uint16_t {
    return static_cast<std::uint16_t>((value & 0xFF) | (((value & 0xFF) - 128) & 0xFF));
}

// jag::oldscape::ServerConnection::Writer_SendClientMessage
static auto hk_fn_send_client_message(void* base, void* callback, client_message* msg) -> void* {
    const auto opcode = msg->opcode;
    const auto size = msg->size;

    const auto payload = std::span<std::uint8_t>(msg->packet.data + 1, size);

    // log::info("opcode {} size {} payload {:02x}", opcode, size, fmt::join(payload, " "));

    // EVENT_MOUSE_CLICK_V1
    if (opcode == 0) {
        mouse_click_v1 click{};
        std::memcpy(&click, payload.data(), sizeof(click));

        click.packed = std::byteswap(click.packed);
        click.x = std::byteswap(click.x);
        click.y = std::byteswap(click.y);

        const auto right = (click.packed & 1) != 0;
        const auto time = click.packed >> 1;

        log::info("right {} time {} x {} y {}", right, time, click.x, click.y);
    }

    // EVENT_MOUSE_CLICK_V2
    if (opcode == 40) {
        mouse_click_v2 click{};
        std::memcpy(&click, payload.data(), sizeof(click));

        click.code = std::byteswap(click.code);
        click.y = read_u16_alt3(click.y);
        click.packed = std::byteswap(click.packed);
        click.x = read_u16_alt3(click.x);

        const auto right = (click.packed & 1) != 0;
        const auto time = click.packed >> 1;

        log::info("code {} right {} time {} x {} y {}", click.code, right, time, click.x, click.y);
    }

    return hk_send_client_message.call<void*>(base, callback, msg);
}

static auto init(std::uint8_t* module) -> void {
    const auto start = std::chrono::steady_clock::now();

    AllocConsole();

    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);

    log::info("osproxy v{} ({}@{}) loaded at {:#x}", META_PROJECT_VERSION, META_GIT_BRANCH,
              META_GIT_COMMIT_SHORT, std::bit_cast<std::uintptr_t>(module));

    log::info("built {} {} with {} {}", __DATE__, __TIME__, META_COMPILER_ID,
              META_COMPILER_VERSION);

    auto buffer = std::string(MAX_PATH, '\0');
    const auto length =
        GetModuleFileNameA(reinterpret_cast<HMODULE>(module), buffer.data(), buffer.size());

    if (length == 0 || length == buffer.size())
        throw std::runtime_error{"failed to get module filename"};

    buffer.resize(length);

    const auto cwd = std::filesystem::path{buffer}.remove_filename();

    log::info("running from directory '{}'", cwd.string());

    const auto list = modules::list();
    const auto osrs = std::ranges::find_if(list, [](auto&& entry) {
        return modules::has_export(entry.base, "AmdPowerXpressRequestHighPerformance");
    });

    if (osrs == list.end())
        throw std::runtime_error{"game executable not found"};

    log::info("detected game executable '{}' at {}", osrs->path.filename().string(),
              fmt::ptr(osrs->base));

    osrs::init(osrs->region());

    hk_send_client_message =
        safetyhook::create_inline(hk_send_client_message_addr, &hk_fn_send_client_message);

    const auto elapsed = std::chrono::steady_clock::now() - start;

    log::info("initialized successfully in {:.2f} seconds",
              std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count());
}

auto DllMain(HMODULE module, DWORD reason, LPVOID) -> BOOL {
    if (reason != DLL_PROCESS_ATTACH)
        return TRUE;

    DisableThreadLibraryCalls(module);

    try {
        init(reinterpret_cast<std::uint8_t*>(module));
    } catch (const std::exception& e) {
        log::warn("initialization failed: {}", e.what());
        return FALSE;
    }

    return TRUE;
}
