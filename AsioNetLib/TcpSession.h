#pragma once
#include <boost/asio.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>
#include "Constants.h"
#include "PacketParser.h"

using namespace boost;
using namespace boost::asio::ip;
using namespace std::chrono;

/// <summary>
/// Tcp ���ǿ��� ����ϴ� ���� ������ ���� Ŭ������ �������̽�
/// </summary>
class ISessionInfo
{};

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
protected:
	tcp::socket socket;
	asio::steady_timer timer;
	bool authValid{false};
	uint32_t sessionId{0};
	PacketParser packetParser;
	ISessionInfo info;
	std::array<char, PACKET_SIZE_MAX> readBuf{0, };
	std::array<char, PACKET_SIZE_MAX> writeBuf{0, };

	signals2::signal<void(int)> onStop;
	signals2::signal<void(size_t)> onRead;
	signals2::signal<void(size_t)> onWrite;

public:
	TcpSession(tcp::socket&& socket, const uint32_t sessionId)
		: socket{std::move(socket)}
		, sessionId{sessionId}
		, timer{socket.get_executor()}
	{}

	virtual ~TcpSession()
	{}

	/// <summary>
	/// ���� �������� �б�/���⸦ ����ϴ� �ڷ�ƾ�� ������
	/// </summary>
	inline void Start()
	{
		// �ڷ�ƾ�� ����Ǵ� ���� �ν��Ͻ��� �����ϱ� ���� shared_from_this()�� �����
		asio::co_spawn(socket.get_executor(),
			[self = shared_from_this()]{return self->ReadAsyncWaitLoop();},
			asio::detached);
	}

	void Stop();

	/// <summary>
	/// ���Ͽ� ������ ���� �ڷ�ƾ�� ����
	/// </summary>
	/// <typeparam name="TPacket">��ŶŸ��</typeparam>
	/// <param name="packet">��Ŷ����</param>
	/// <returns></returns>
	template <typename TPacket>
	asio::awaitable<void> WriteAsync(TPacket&& packet)
	{
		PACKET_DATA_TYPE_CHECK<TPacket>();
		try
		{
			writeBuf.fill(0);
			std::size_t packetSize = Packet::Serialize<TPacket>(writeBuf.data(), std::move(packet));
			co_await asio::async_write(
				socket,
				asio::buffer(writeBuf.data(), packetSize),
				asio::use_awaitable);
			onWrite(packetSize);
		}
		catch ( std::exception& ex )
		{
			Stop();
		}
	}

	/// <summary>
	/// ������ ����� �� ȣ��Ǵ� �̺�Ʈ ����
	/// </summary>
	/// <param name="event"> sessionId </param>
	/// <returns></returns>
	auto AddOnStopEvent(signals2::signal<void(int)>::slot_type&& event)
	{
		onStop.connect(event);
	}

	/// <summary>
	/// ���Ͽ��� �����͸� ���� �� ȣ��Ǵ� �̺�Ʈ ����
	/// </summary>
	/// <param name="event">���� ����Ʈ ��</param>
	/// <returns></returns>
	auto AddOnReadEvent(signals2::signal<void(size_t)>::slot_type&& event)
	{
		onRead.connect(event);
	}

	/// <summary>
	/// ���Ͽ� �����͸� �� �� ȣ��Ǵ� �̺�Ʈ ����
	/// </summary>
	/// <param name="event">�� ����Ʈ ��</param>
	/// <returns></returns>
	auto AddOnWriteEvent(signals2::signal<void(size_t)>::slot_type&& event)
	{
		onWrite.connect(event);
	}

	template<typename T>
	inline T& SessionInfo()
	{
		return dynamic_cast<T&>(info);
	}

	template<typename T>
	inline void SessionInfo(T&& info)
	{
		return this->info = dynamic_cast<T>(info);
	}

private:
	/// <summary>
	/// ������ ������ �б⸦ ����ϴ� ����
	/// </summary>
	/// <returns></returns>
	asio::awaitable<void> ReadAsyncWaitLoop();

	/// <summary>
	/// ���Ͽ��� ���� �����͸� �м��Ͽ� �ش��ϴ� ��Ʈ�ѷ��� ����
	/// </summary>
	/// <param name="packetId"></param>
	/// <param name="dataBuf"></param>
	/// <returns></returns>
	asio::awaitable<void> Dispatch(const PacketIdType packetId, msgpack::sbuffer& dataBuf);

	inline void Id(const uint32_t id)
	{
		sessionId = id;
	}
	inline const uint32_t Id() const
	{
		return sessionId;
	}
	inline void Auth(const bool valid)
	{
		authValid = valid;
	}
};
