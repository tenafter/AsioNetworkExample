#pragma once

#include <msgpack.hpp>
#include <boost/asio.hpp>
#include <Constants.h>
#include <TcpClient.h>
using namespace boost;

/// <summary>
/// 클라이언트 측 인증 응답 패킷 처리
/// </summary>
class AuthController
{
public:
	static asio::awaitable<void> AuthRes(
		TcpClient& client, msgpack::sbuffer& dataBuf);

	static void RegisterFunc();
};
