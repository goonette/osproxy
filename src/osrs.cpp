#include "osrs.hpp"

#include "memory.hpp"
#include "console.hpp"

using namespace console;

namespace {

template <typename T = std::uint8_t*>
auto follow_find(auto region, auto pattern) -> T {
    return reinterpret_cast<T>(memory::follow(memory::find(region, pattern)));
}

} // namespace

auto osrs::init(std::span<std::uint8_t> region) -> void {
    hk_send_client_message_addr = memory::find(region, "40 53 48 83 EC ? 48 8B DA 49 8B C0");
    log::misc("found send client message function at {}", fmt::ptr(hk_send_client_message_addr));
}