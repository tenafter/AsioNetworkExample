#pragma once

//#include <msgpack.hpp>
#include <boost/asio.hpp>
#include "Constants.h"

using namespace boost;

namespace Packet
{
	/// <summary>
	/// 패킷데이터가 담긴 버퍼를 패킷데이터의 인스턴스로 변환
	/// </summary>
	/// <typeparam name="TPacket">패킷데이터 구조체</typeparam>
	/// <param name="buffer">패킷데이터의 버퍼</param>
	/// <param name="size">패킷데이터의 크기</param>
	/// <returns></returns>
	template <typename TPacket>
	inline static auto Deserialize(const char* buffer, const size_t size)
	{
		PACKET_DATA_TYPE_CHECK<TPacket>();
		msgpack::object_handle handle = msgpack::unpack(buffer, size);
		return handle.get().as<TPacket>();
	}

	/// <summary>
	/// 패킷데이터와 헤더를 결합해서 패킷을 만든다
	/// 패킷데이터 타입만 허용되며, 그 외에는 컴파일이 되지 않음
	/// </summary>
	/// <typeparam name="TPacket">패킷데이터 구조체</typeparam>
	/// <param name="packetBuf">패킷을 담을 버퍼</param>
	/// <param name="packetData">패킷데이터 구조체의 인스턴스</param>
	/// <returns></returns>
	template <typename TPacket>
	inline static const size_t Serialize(char* packetBuf, TPacket&& packetData)
	{
		PACKET_DATA_TYPE_CHECK<TPacket>();
		msgpack::sbuffer dataBuf(PACKET_SIZE_MAX);
		msgpack::pack(dataBuf, packetData);

		Header header;
		std::memcpy(header.start, STARTER, STARTER_SIZE);
		header.packetType = static_cast<short>(packetData.packetType);
		header.dataSize = static_cast<uint16_t>(dataBuf.size());
		std::memcpy(static_cast<void*>(packetBuf), &header, HEADER_SIZE);
		std::memcpy(packetBuf + sizeof(Header), dataBuf.data(), dataBuf.size());
		return HEADER_SIZE + dataBuf.size();
	}

	/// <summary>
	/// 버퍼로 부터 헤더를 추출해서 반환함
	/// </summary>
	/// <param name="packetBuf">패킷 버퍼</param>
	/// <returns>패킷 헤더</returns>
	inline static Header ReadHeader(const char* packetBuf)
	{
		Header header;
		std::memcpy(&header, packetBuf, HEADER_SIZE);
		return header;
	}
}
