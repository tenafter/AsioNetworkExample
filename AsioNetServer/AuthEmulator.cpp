#include "AuthEmulator.h"
#include <CommonConstants.h>
#include <iostream>

asio::awaitable<int64_t> AuthEmulator::Authorize(std::string accessToken)
{
	uint64_t zoneId = ZONE_ID_NONE;
	co_await asio::co_spawn(*ioc, [&zoneId, this] ()->asio::awaitable<void>
	{
		asio::steady_timer timer(*this->ioc);
		timer.expires_from_now(interval);
		system::error_code ec;
		co_await timer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
		if ( ec )
		{
			std::cout << ec.message() << std::endl;
			zoneId = ZONE_ID_NONE;
		}
		else
		{
			zoneId = zoneRange(mtRandom);
		}
	}
	, asio::use_awaitable);
	co_return zoneId;
}

