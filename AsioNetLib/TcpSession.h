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
/// Tcp 세션에서 사용하는 유저 정보를 담은 클래스의 인터페이스
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
	/// 소켓 데이터의 읽기/쓰기를 대기하는 코루틴을 시작함
	/// </summary>
	inline void Start()
	{
		// 코루틴이 실행되는 동안 인스턴스를 유지하기 위해 shared_from_this()를 사용함
		asio::co_spawn(socket.get_executor(),
			[self = shared_from_this()]{return self->ReadAsyncWaitLoop();},
			asio::detached);
	}

	void Stop();

	/// <summary>
	/// 소켓에 데이터 쓰는 코루틴을 시작
	/// </summary>
	/// <typeparam name="TPacket">패킷타입</typeparam>
	/// <param name="packet">패킷내용</param>
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
	/// 세션이 종료될 때 호출되는 이벤트 정의
	/// </summary>
	/// <param name="event"> sessionId </param>
	/// <returns></returns>
	auto AddOnStopEvent(signals2::signal<void(int)>::slot_type&& event)
	{
		onStop.connect(event);
	}

	/// <summary>
	/// 소켓에서 데이터를 읽을 때 호출되는 이벤트 정의
	/// </summary>
	/// <param name="event">읽은 바이트 수</param>
	/// <returns></returns>
	auto AddOnReadEvent(signals2::signal<void(size_t)>::slot_type&& event)
	{
		onRead.connect(event);
	}

	/// <summary>
	/// 소켓에 데이터를 쓸 때 호출되는 이벤트 정의
	/// </summary>
	/// <param name="event">쓴 바이트 수</param>
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
	/// 소켓의 데이터 읽기를 대기하는 루프
	/// </summary>
	/// <returns></returns>
	asio::awaitable<void> ReadAsyncWaitLoop();

	/// <summary>
	/// 소켓에서 읽은 데이터를 분석하여 해당하는 컨트롤러에 전달
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
