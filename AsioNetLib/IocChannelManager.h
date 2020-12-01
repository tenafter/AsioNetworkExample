#pragma once
#include <vector>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "IocChannel.h"

/// <summary>
/// 채널 스레드 매니저
/// </summary>
class IocChannelManager
{
private:
	std::vector<std::unique_ptr<IocChannel>> channels;

public:
	static IocChannelManager& Inst()
	{
		static IocChannelManager instance;
		return instance;
	}

	void Init(int channelThreadCnt)
	{
		for ( int i = 0; i < channelThreadCnt; ++i )
		{
			channels.emplace_back(std::make_unique<IocChannel>(channelThreadCnt));
			channels.back()->Run();
		}
	}

	inline auto& GetChannel(int zoneId)
	{
		return *channels.at(zoneId % channels.size());
	}
};

