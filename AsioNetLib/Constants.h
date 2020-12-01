#pragma once
#include <cstdint>

inline constexpr int PACKET_SIZE_MAX{4096};
inline constexpr int STARTER_SIZE = 2;

using PacketIdType = uint16_t;
inline constexpr uint16_t PACKET_TYPE_NONE = 0;
inline constexpr uint16_t CHANNEL_MESSAGE = 1000;

#pragma pack(push, 1)
inline constexpr char STARTER[STARTER_SIZE] = {'P', 'K'};
struct Header
{
	char start[2];
	uint16_t dataSize = 0;
	PacketIdType packetType = 0;
};
#pragma pack(pop)

inline constexpr int HEADER_SIZE{sizeof(Header)};

inline constexpr int SESSION_COUNT_MAX{20000};

namespace Packet
{
	/// <summary>
	/// TPacket 템플릿 타입 검사를 위해 사용함
	/// </summary>
	struct PacketData
	{};
};

template <typename TPacket>
constexpr void PACKET_DATA_TYPE_CHECK()
{
	static_assert(std::is_base_of<Packet::PacketData, TPacket>::value == true, "Plaese, Use PacketData Type");
}

