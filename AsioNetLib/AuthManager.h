#pragma once
#include <boost/asio.hpp>
#include <string>
#include <chrono>
#include "TcpServer.h"
using namespace boost;
using namespace std::chrono_literals;

class AuthManager
{
	std::shared_ptr<asio::io_context> ioc;
public:
	static AuthManager& Inst()
	{
		static AuthManager instance;
		return instance;
	}

	AuthManager() : ioc( std::make_shared<asio::io_context>() )
	{
		std::thread( [this]
			{
				this->ioc->run();
			} );
	}

	//asio::awaitable<bool> AuthAsync(std::shared_ptr<TcpSession> session, std::string userKey) 
	//{
	//	asio::steady_timer timer(*ioc);
	//	system::error_code ec;
	//	co_await timer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
	//	co_return true;
	//	//session->Auth(true);
	//}
};

