#include "pch.h"
#include <array>
#include <Constants.h>
#include <PacketDefine.h>
#include <PacketUtil.h>

/// <summary>
/// 패킷 시리얼라이즈/디시리얼라이즈 테스트
/// </summary>
TEST(PacketTest, Serialize)
{
	Packet::EchoTestReq packetObject;
	packetObject.seqId = 1000;
	packetObject.testString = "AABBCCDD";

	std::array<char, 1024> packetBuf;
	packetBuf.fill(0);
	Packet::Serialize<Packet::EchoTestReq>(packetBuf.data(), std::move(packetObject));

	Header header = Packet::ReadHeader(packetBuf.data());
	EXPECT_EQ(packetObject.packetType, header.packetType);

	auto packetObjectDes = Packet::Deserialize<Packet::EchoTestReq>(packetBuf.data() + HEADER_SIZE, header.dataSize);
	EXPECT_EQ(packetObject.seqId, packetObjectDes.seqId);
	EXPECT_EQ(packetObject.testString, packetObjectDes.testString);
}

