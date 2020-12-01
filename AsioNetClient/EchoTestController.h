#pragma once
#include <msgpack.hpp>
#include <boost/asio.hpp>
#include <TcpClient.h>
using namespace boost;

/// <summary>
/// 클라이언트 사이드 에코 테스트 프로토콜 응답 처리
/// </summary>
class EchoTestController
{
public:
	static asio::awaitable<void> TestChannelEchoResFunc(
		TcpClient& client, msgpack::sbuffer& dataBuf);

	static void RegisterFunc();
};