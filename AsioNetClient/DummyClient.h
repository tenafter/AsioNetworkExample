#pragma once
#include <PacketUtil.h>
#include <PacketDefine.h>
#include <ClientControllerMap.h>
#include <TcpClient.h>
using namespace std::chrono;
using namespace std::chrono_literals;

inline static int gClientLoopInterval{1000};

class DummyClient : public TcpClient
{
private:
	enum Status : int
	{
		TryAuth,
		TryZoneJoin,
		TryZoneDoAction,
		TryZoneLeave,
	};

	asio::steady_timer timer;
	double totalMs{0}, avarage{0};
	uint64_t count{0};
	uint64_t zoneId{0};

public:
	DummyClient(asio::io_context& ioc) : TcpClient{ioc}, timer{ioc}{}
	~DummyClient()
	{}

	const double TotalMs()
	{
		return totalMs;
	}
	const double AvarageMs()
	{
		return avarage;
	}
	const uint64_t TotalCount()
	{
		return count;
	}

	void AddEchoState(double responseMs)
	{
		totalMs += responseMs;
		++count;
		avarage = totalMs / count;
	}

	void SetZoneId(uint64_t zoneId)
	{
		this->zoneId = zoneId;
	}

protected:
	asio::awaitable<void> MainLoop()
	{
		auto prevTime = system_clock::now();
		while ( socket.is_open() == true )
		{
			timer.expires_from_now(milliseconds(gClientLoopInterval));
			system::error_code ec;
			co_await timer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
			if ( ec )
			{
				break;
			}
			co_await DoEcho();
		}
		co_return;
	}

	/// <summary>
	/// 에코 패킷을 전송함
	/// </summary>
	/// <returns></returns>
	asio::awaitable<void> DoEcho()
	{
		Packet::EchoTestReq packet;
		packet.seqId = ++seqNumber;
		packet.testString = "Client Data";
		packet.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		// 주의 co_await 키워드가 없으면 이 루프는 코루틴이 되지 않고, 무한 루프를 돌게 된다.
		co_await WriteDataAsync<Packet::EchoTestReq>(std::move(packet));
	}

	/// <summary>
	/// 인증 패킷을 전송함
	/// </summary>
	/// <returns></returns>
	asio::awaitable<void> DoZoneAction()
	{
		Packet::AuthReq packet;
		packet.accessToken = "";

		// 주의 co_await 키워드가 없으면 이 루프는 코루틴이 되지 않고, 무한 루프를 돌게 된다.
		co_await WriteDataAsync<Packet::AuthReq>(std::move(packet));
	}
};