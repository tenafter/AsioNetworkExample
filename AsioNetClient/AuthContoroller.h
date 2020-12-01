#pragma once

#include <msgpack.hpp>
#include <boost/asio.hpp>
#include <Constants.h>
#include <TcpClient.h>
using namespace boost;

/// <summary>
/// Ŭ���̾�Ʈ �� ���� ���� ��Ŷ ó��
/// </summary>
class AuthController
{
public:
	static asio::awaitable<void> AuthRes(
		TcpClient& client, msgpack::sbuffer& dataBuf);

	static void RegisterFunc();
};
