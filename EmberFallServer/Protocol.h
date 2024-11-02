#pragma once

inline constexpr size_t CHAT_PACKET_MAX_SIZE = 100;
inline constexpr unsigned char NULLID = 0xff;
inline constexpr unsigned char MAX_CLIENT = 16;

/// <summary>
/// SC -> From Server To Client
/// CS -> From Client To Server
/// </summary>
enum PacketType {
    // SC Part
    PT_SC_PacketConnected,
    PT_SC_PacketChatting,

    // CS Part
    PT_CS_PacketChatting,
};

#pragma pack(push, 1)
struct Packet {
    unsigned char size;
    unsigned char type;
    unsigned char id;
};

struct PacketConnected : public Packet { };

struct PacketChatting : public Packet {
    char chatBuffer[CHAT_PACKET_MAX_SIZE];
};
#pragma pack(pop)