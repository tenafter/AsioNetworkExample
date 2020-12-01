#pragma once
#include <iostream>
#include <chrono>
#include <atomic>
#include <functional>
#include <boost/asio.hpp>
#include "SessionManager.h"
using namespace boost;
using namespace std::chrono;
using namespace std::chrono_literals;

class StatusMonitor
{
private:
	bool isInit{false};
	size_t connections{0};
	std::atomic<size_t> totalWriteSize{0};
	std::atomic<size_t> totalWriteCount{0};
	std::atomic<size_t> lastWriteSize{0};
	std::atomic<size_t> lastWriteCount{0};
	std::atomic<size_t> totalReadSize{0};
	std::atomic<size_t> totalReadCount{0};
	std::atomic<size_t> lastReadSize{0};
	std::atomic<size_t> lastReadCount{0};

	int show_title = 0;
	system_clock::time_point start_time{system_clock::now()};
	system_clock::time_point last_time{system_clock::now()};
	int networkThreadCount{0};
	int channelThreadCount{0};

	seconds interval{1};

	StatusMonitor()
	{};

public:
	static StatusMonitor& Inst()
	{
		static StatusMonitor inst;
		return inst;
	}

	void Start(asio::io_context& ioc, int networkThreadCount, int channelThreadCount);

private:
	asio::awaitable<void> ShowLoop(asio::io_context& ioc);

	void Show();
};

