#pragma once
#include <msgpack.hpp>
#include <boost/asio.hpp>
#include <TcpClient.h>
using namespace boost;

/// <summary>
/// Ŭ���̾�Ʈ ���̵� ���� �׽�Ʈ �������� ���� ó��
/// </summary>
class EchoTestController
{
public:
	static asio::awaitable<void> TestChannelEchoResFunc(
		TcpClient& client, msgpack::sbuffer& dataBuf);

	static void RegisterFunc();
};