#include <tbb/tbbmalloc_proxy.h>
#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <TcpServer.h>
#include <TcpClient.h>
#include "EchoTestController.h"
#include "AuthContoroller.h"
#include "StatusMonitor.h"
namespace po = boost::program_options;

std::tuple<int, unsigned short, int, int> ProgramOptions(const int argc, char* argv[])
{
	int exit{1}, port{43234}, ioThreadCnt{2}, channelThreadCnt{2};
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "help message")
		("port", po::value<unsigned short>(), "server port")
		("ioThreadCnt", po::value<int>(), "네트워크 io 스레드 수")
		("channelThreadCnt", po::value<int>(), "채널을 처리할 스레드 수");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	if ( vm.size() == 0 || vm.count("help") )
	{
		std::cout << "Usage : \n" << desc << std::endl;
		exit = 0;
	}
	else
	{
		if ( vm.count("port") )
		{
			port = vm["port"].as<unsigned short>();
		}
		if ( vm.count("ioThreadCnt") )
		{
			ioThreadCnt = vm["ioThreadCnt"].as<int>();
		}
		if ( vm.count("channelThreadCnt") )
		{
			channelThreadCnt = vm["channelThreadCnt"].as<int>();
		}
	}
	return {exit, port, ioThreadCnt, channelThreadCnt};
}

int main(const int argc, char* argv[])
{
	auto options = ProgramOptions(argc, argv);
	// exit
	if ( get<0>(options) == 0 )
	{
		char input;
		std::cin >> input;
		return 0;
	}

	EchoTestController::RegisterFunc();
	AuthController::RegisterFunc();

	TcpServer server;
	std::async(std::launch::deferred,
		[&server, options] ()
	{
		auto [exit, port, ioThreadCnt, channelThreadCnt] = options;
		server.Init(port, ioThreadCnt, channelThreadCnt);
		StatusMonitor::Inst().Start(server.IoContext(), ioThreadCnt, channelThreadCnt);
		server.Start();
	})
		.wait();

	return EXIT_SUCCESS;
}
