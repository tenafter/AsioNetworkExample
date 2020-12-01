#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <random>
using namespace boost;
using namespace std::chrono;
using namespace std::chrono_literals;

constexpr milliseconds interval = 100ms;
constexpr uint64_t ZONE_ID_MIN = 1001;
constexpr uint64_t ZONE_ID_MAX = 1010;

class AuthEmulator
{
private:
	std::unique_ptr<asio::io_context> ioc;
	asio::executor_work_guard<asio::io_context::executor_type> work_guard;
	std::thread iocThread;
	asio::steady_timer timer;

	// 존아이디를 랜덤으로 ..
	std::random_device rd;
	std::mt19937_64 mtRandom;
	std::uniform_int_distribution<int> zoneRange;

	AuthEmulator() : ioc(std::make_unique<asio::io_context>()),
		work_guard(asio::make_work_guard(*ioc)),
		timer(*ioc), mtRandom(rd()),
		zoneRange(ZONE_ID_MIN, ZONE_ID_MAX)
	{
		iocThread = std::thread([this] ()
		{
			ioc->run();
		});
	}

public:
	static AuthEmulator& Inst()
	{
		static AuthEmulator instance;
		return instance;
	}

	asio::awaitable<int64_t> Authorize(std::string accessToken);
};

