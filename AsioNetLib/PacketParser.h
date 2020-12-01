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
	/// ��Ŷ�� ��ȿ���� �˻�
	/// �߸� ��Ŷ�� �����ߴٰ�, ��ģ��.
	/// ���� ��Ŷ (������ ���� ��) �� ���ܸ� �߻���Ų��
	/// </summary>
	/// <param name="readBuf">���Ź���</param>
	/// <param name="byteTransferred">���Ź���Ʈ ��</param>
	/// <returns>��������, ��ŶŸ��</returns>
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
				throw std::exception("��Ŷ ��� ����");
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

