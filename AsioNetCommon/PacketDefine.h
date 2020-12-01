#pragma once

#include <msgpack.hpp>
#include "PacketType.h"

namespace Packet
{
	struct EchoTestReq : public PacketData
	{
		PacketIdType packetType = PacketId::EchoTestReq;
		int seqId;
		std::string testString;
		std::array<char, 1024> bin{0, };
		int64_t timestamp; // microseconds
		MSGPACK_DEFINE(packetType, seqId, testString, bin, timestamp);
	};

	struct EchoTestRes : public PacketData
	{
		PacketIdType packetType = PacketId::EchoTestRes;
		int seqId;
		std::string testString;
		std::array<char, 1024> bin{0, };
		int64_t timestamp; // microseconds
		//uint64_t serverProcessTime; // microseconds
		MSGPACK_DEFINE(packetType, seqId, testString, bin, timestamp);
	};

	struct AuthReq : public PacketData
	{
		PacketIdType packetType = PacketId::AuthReq;
		std::string accessToken;
		MSGPACK_DEFINE(packetType, accessToken);
	};

	struct AuthRes : public PacketData
	{
		PacketIdType packetType = PacketId::AuthRes;
		int64_t zoneId;
		MSGPACK_DEFINE(packetType, zoneId);
	};

	struct ZoneJoinReq : public PacketData
	{
		PacketIdType packetType = PacketId::ZoneJoinReq;
		int64_t zoneId;
		MSGPACK_DEFINE(packetType, zoneId);
	};

	struct ZoneJoinRes : public PacketData
	{
		PacketIdType packetType = PacketId::ZoneJoinRes;
		int64_t zoneId;
		MSGPACK_DEFINE(packetType, zoneId);
	};

	struct ZoneLeaveReq : public PacketData
	{
		PacketIdType packetType = PacketId::ZoneLeaveReq;
		MSGPACK_DEFINE(packetType);
	};

	struct ZoneLeaveRes : public PacketData
	{
		PacketIdType packetType = PacketId::ZoneLeaveRes;
		MSGPACK_DEFINE(packetType);
	};
}
