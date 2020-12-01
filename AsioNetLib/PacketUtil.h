#pragma once

//#include <msgpack.hpp>
#include <boost/asio.hpp>
#include "Constants.h"

using namespace boost;

namespace Packet
{
	/// <summary>
	/// ��Ŷ�����Ͱ� ��� ���۸� ��Ŷ�������� �ν��Ͻ��� ��ȯ
	/// </summary>
	/// <typeparam name="TPacket">��Ŷ������ ����ü</typeparam>
	/// <param name="buffer">��Ŷ�������� ����</param>
	/// <param name="size">��Ŷ�������� ũ��</param>
	/// <returns></returns>
	template <typename TPacket>
	inline static auto Deserialize(const char* buffer, const size_t size)
	{
		PACKET_DATA_TYPE_CHECK<TPacket>();
		msgpack::object_handle handle = msgpack::unpack(buffer, size);
		return handle.get().as<TPacket>();
	}

	/// <summary>
	/// ��Ŷ�����Ϳ� ����� �����ؼ� ��Ŷ�� �����
	/// ��Ŷ������ Ÿ�Ը� ���Ǹ�, �� �ܿ��� �������� ���� ����
	/// </summary>
	/// <typeparam name="TPacket">��Ŷ������ ����ü</typeparam>
	/// <param name="packetBuf">��Ŷ�� ���� ����</param>
	/// <param name="packetData">��Ŷ������ ����ü�� �ν��Ͻ�</param>
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
	/// ���۷� ���� ����� �����ؼ� ��ȯ��
	/// </summary>
	/// <param name="packetBuf">��Ŷ ����</param>
	/// <returns>��Ŷ ���</returns>
	inline static Header ReadHeader(const char* packetBuf)
	{
		Header header;
		std::memcpy(&header, packetBuf, HEADER_SIZE);
		return header;
	}
}
