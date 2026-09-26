#pragma once

#include <span>

namespace osrs {

enum client_opcodes : std::uint32_t {
    EVENT_MOUSE_CLICK_V1 = 0,
    EVENT_MOUSE_CLICK_V2 = 40,

    MOVE_GAMECLICK = 102,
};

struct isaac {
    /* 0x0000 */ std::uint32_t count;
    /* 0x0004 */ std::uint32_t results[256];
    /* 0x0400 */ std::uint32_t mem[256];
    /* 0x0324 */ std::uint32_t a;
    /* 0x0328 */ std::uint32_t b;
    /* 0x032C */ std::uint32_t c;
};
static_assert(sizeof(isaac) == 0x810);

struct packet {
    /* 0x0000 */ std::uint8_t pad_0000[8];
    /* 0x0008 */ std::int64_t capacity;
    /* 0x0010 */ std::uint8_t* data;
    /* 0x0018 */ std::int64_t pos;
    /* 0x0020 */ std::uint8_t pad_0020[8];
    /* 0x0028 */ isaac* cipher;
};
static_assert(sizeof(packet) == 0x30);

struct message {
    /* 0x0000 */ std::uint32_t opcode;
    /* 0x0004 */ std::int32_t size;
    /* 0x0008 */ packet packet;
    /* 0x0038 */ std::uint8_t pad_0038[4];
};
static_assert(sizeof(message) == 0x40);

struct mouse_click_v1 {
    /* 0x0000 */ std::uint16_t packed;
    /* 0x0002 */ std::uint16_t x;
    /* 0x0004 */ std::uint16_t y;
};
static_assert(sizeof(mouse_click_v1) == 0x6);

#pragma pack(push, 1)
struct mouse_click_v2 {
    /* 0x0000 */ std::uint8_t code;
    /* 0x0001 */ std::uint16_t y;
    /* 0x0003 */ std::uint16_t packed;
    /* 0x0005 */ std::uint16_t x;
};
static_assert(sizeof(mouse_click_v2) == 0x7);

struct move_game_click {
    /* 0x0000 */ std::uint16_t y;
    /* 0x0002 */ std::uint8_t key_combo;
    /* 0x0003 */ std::uint16_t x;
};
static_assert(sizeof(move_game_click) == 0x5);
#pragma pack(pop)

inline std::uint8_t* hk_send_client_message_addr = {};

auto init(std::span<std::uint8_t> region) -> void;

} // namespace osrs
