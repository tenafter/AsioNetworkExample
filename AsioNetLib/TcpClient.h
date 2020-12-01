#pragma once
#include <boost/asio.hpp>
#include "Constants.h"
#include "PacketParser.h"
using namespace boost;
using namespace boost::asio::ip;

/// <summary>
/// 더미 테스트용 클라이언트 베이스 클래스
/// </summary>
class TcpClient
{
protected:
	tcp::socket socket;
	asio::io_context& ioc;
	int seqNumber{0};
	std::array<char, PACKET_SIZE_MAX> readBuf{0, };
	std::array<char, PACKET_SIZE_MAX> writeBuf{0, };
	asio::steady_timer timer;
	PacketParser packetParser;

	asio::awaitable<void> ReadAsyncWaitLoop();

	asio::awaitable<void> Dispatch(const PacketIdType packetId, msgpack::sbuffer& dataBuf);

	/// <summary>
	/// 클라이언트가 접속 동안 할일을 정의함
	/// </summary>
	virtual asio::awaitable<void> MainLoop() = 0;

	template <typename TPacket>
	asio::awaitable<void> WriteDataAsync(TPacket&& packetData)
	{
		PACKET_DATA_TYPE_CHECK<TPacket>();
		if ( socket.is_open() == false )
		{
			co_return;
		}
		size_t packetSize = Packet::Serialize<TPacket>(writeBuf.data(), std::move(packetData));
		co_await WriteBufferAsync(writeBuf.data(), packetSize);
		writeBuf.fill(0);
	}

	asio::awaitable<void> WriteBufferAsync(const char* packetBuf, size_t packetBufSize);

public:
	TcpClient(asio::io_context& ioc) : ioc{ioc}, socket{ioc}, timer{ioc} {}

	virtual ~TcpClient()
	{}

	void Connect();

	void Close();
};


