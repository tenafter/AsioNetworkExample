#pragma once

#include <msgpack.hpp>
#include <boost/asio.hpp>
#include <Constants.h>
using namespace boost;

class AuthController
{
public:
	static asio::awaitable<void> AuthReq(
		uint32_t sessionId, msgpack::sbuffer& dataBuf);

	static void RegisterFunc();
};
