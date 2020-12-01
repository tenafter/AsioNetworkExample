#pragma once
#include <msgpack.hpp>
#include "Constants.h"
#include "PacketUtil.h"
using namespace Packet;

class PacketParser
{
private:
	const int PARTIAL_PACKET_SIZE_MAX = PACKET_SIZE_MAX * 2;
	msgpack::sbuffer partialBuf{PARTIAL_PACKET_SIZE_MAX};
	msgpack::sbuffer packetData{PACKET_SIZE_MAX};

	Header header;
public:
	inline msgpack::sbuffer& PacketData()
	{
		return packetData;
	}

	/// <summary>
	/// 패킷을 유효성을 검사
	/// 잘린 패킷은 보관했다가, 합친다.
	/// 오류 패킷 (구분자 오류 등) 을 예외를 발생시킨다
	/// </summary>
	/// <param name="readBuf">수신버퍼</param>
	/// <param name="byteTransferred">수신바이트 수</param>
	/// <returns>성공여부, 패킷타입</returns>
	inline std::pair<bool, PacketIdType> PacketParse(std::array<char, PACKET_SIZE_MAX>& readBuf, const std::size_t byteTransferred)
	{
		bool sucess = false;
		packetData.clear();
		partialBuf.write(readBuf.data(), byteTransferred);
		if ( partialBuf.size() >= HEADER_SIZE )
		{
			std::memset(&header, 0, HEADER_SIZE);
			header = ReadHeader(partialBuf.data());
			const int dataSizeTotal = header.dataSize + HEADER_SIZE;
			// invalid
			if ( std::memcmp(&header.start, STARTER, STARTER_SIZE) != 0 )
			{
				throw std::exception("패킷 헤더 오류");
			}
			// short
			if ( partialBuf.size() < dataSizeTotal )
			{
				sucess = false;
			}
			// valid, but long
			else if ( partialBuf.size() > dataSizeTotal )
			{
				packetData.write(partialBuf.data() + HEADER_SIZE, dataSizeTotal);
				//tempBuf.clear();
				msgpack::sbuffer tempBuf(partialBuf.size() - dataSizeTotal);
				tempBuf.write(partialBuf.data() + header.dataSize + HEADER_SIZE, partialBuf.size() - dataSizeTotal);
				partialBuf.clear();
				partialBuf.write(tempBuf.data(), tempBuf.size());
				sucess = true;
			}
			// valid
			else
			{
				packetData.write(partialBuf.data() + HEADER_SIZE, dataSizeTotal);
				partialBuf.clear();
				sucess = true;
			}
		}
		return {sucess, header.packetType};
	}
};

