#pragma once

#include <msgpack.hpp>
#include <boost/asio.hpp>
#include <Constants.h>
using namespace boost;

class EchoTestController
{
public:
	static asio::awaitable<void> EchoTestReq(
		const uint32_t sessionId, msgpack::sbuffer& dataBuf);

	static void RegisterFunc();
};