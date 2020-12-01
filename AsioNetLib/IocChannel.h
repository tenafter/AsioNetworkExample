#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <thread>
using namespace boost;
using namespace boost::asio::ip;

class IocChannel
{
private:
	std::unique_ptr<asio::io_context> ioc;
	asio::executor_work_guard<asio::io_context::executor_type> work_guard;
	std::thread iocThread;

public:
	IocChannel(int channelThreaCnt) : ioc(std::make_unique<asio::io_context>(channelThreaCnt)),
		work_guard(asio::make_work_guard(*ioc))
	{}

	void Run()
	{
		iocThread = std::thread([this] ()
		{
			ioc->run();
		});
	}

	void Stop()
	{
		ioc->stop();
		if ( iocThread.joinable() )
		{
			iocThread.join();
		}
	}

	inline auto& IocExecutor()
	{
		return *ioc;
	}
};

