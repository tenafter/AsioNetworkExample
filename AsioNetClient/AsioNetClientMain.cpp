﻿#include <tbb/tbbmalloc_proxy.h>
#include <iostream>
#include "EchoTestController.h"
#include <boost/program_options.hpp>
#include "DummyClient.h"
#include "DummyClientManager.h"
#include "AuthContoroller.h"
namespace po = boost::program_options;

std::tuple<int, std::string, uint16_t, int, int, int> ProgramOptions(int argc, char* argv[])
{
	std::string ip{"127.0.0.1"};
	int exit{1}, port{43234}, clientCnt{10}, threadCnt{2}, interval{1000};
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "help message")
		("ip", po::value<std::string>(), "server ip")
		("port", po::value<uint16_t>(), "server port")
		("clientCnt", po::value<int>(), "client connection count")
		("threadCnt", po::value<int>(), "thread count")
		("interval", po::value<int>(), "echo packet send interval milliseconds")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	if ( vm.size() == 0 || vm.count("help") )
	{
		std::cout << "Usage : \n" << desc << std::endl;
		exit = 0;
	}
	else
	{
		if ( vm.count("ip") )
		{
			ip = vm["ip"].as<std::string>();
		}
		if ( vm.count("port") )
		{
			port = vm["port"].as<uint16_t>();
		}
		if ( vm.count("clientCnt") )
		{
			clientCnt = vm["clientCnt"].as<int>();
		}
		if ( vm.count("threadCnt") )
		{
			threadCnt = vm["threadCnt"].as<int>();
		}
		if ( vm.count("interval") )
		{
			interval = vm["interval"].as<int>();
		}
	}
	return {exit, ip, port, clientCnt, threadCnt, interval};
}

class ClientMonitor : public std::enable_shared_from_this<ClientMonitor>
{
private:
	DummyClientManager<DummyClient>& cliManager;
	std::thread workThread;
	bool isStopped{false};

public:
	ClientMonitor(DummyClientManager<DummyClient>& cliManager) : cliManager{cliManager}
	{}

	~ClientMonitor()
	{
		isStopped = true;
		if ( workThread.joinable() )
		{
			workThread.join();
		}
	}

	void Start()
	{
		workThread = std::thread([self = shared_from_this()]() {
			while ( self->isStopped == false )
			{
				std::this_thread::sleep_for(500ms);
				int totalClients = 0;
				double averageResponseMs = 0, totalAverageResponeMs = 0;
				uint64_t totalEchoCount = 0;
				for ( auto& group : self->cliManager.Groups() )
				{
					for ( auto& client : group->Clients() )
					{
						if ( client->TotalCount() > 0 )
						{
							totalEchoCount += client->TotalCount();
							totalAverageResponeMs += client->AvarageMs();
							++totalClients;
						}
						std::this_thread::yield();
					}
				}
				averageResponseMs = totalClients > 0 ? totalAverageResponeMs / totalClients : 0;
				printf("Echo Info totalClients: %d, averageResponseMs: %lf, totalEchoCount: %lld\n"
					, totalClients, averageResponseMs, totalEchoCount);
			}
		});
	}
};

int main(int argc, char* argv[])
{
	auto [exit, ip, port, clientCnt, threadCnt, interval] = ProgramOptions(argc, argv);
	if ( exit == 0 )
	{
		char input;
		std::cin >> input;
		return 0;
	}

	// 더미 클라이언트 루프 반복 간격
	gClientLoopInterval = interval;

	// 패킷 컨트롤러 등록
	EchoTestController::RegisterFunc();
	AuthController::RegisterFunc();

	// 더미클라이언트 실행 및 더미클라이언트 모니터링 설정
	DummyClientManager<DummyClient> cliManager;
	auto monitor = std::make_shared<ClientMonitor>(cliManager);
	monitor->Start();
	cliManager.StartClients(ip, port, clientCnt, threadCnt);
}
