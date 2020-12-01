#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <thread>
using namespace boost;

/// <summary>
/// 더미 클라이언트 관리
/// </summary>
/// <typeparam name="TClient">클라이언트 템플릿</typeparam>
template <typename TClient>
class DummyClientManager
{
private:
	template <typename TClient>
	class ClientGroup
	{
	private:
		std::unique_ptr<asio::io_context> ioc;
		asio::executor_work_guard<asio::io_context::executor_type> work_guard;
		std::thread workThread;
		int clientCount;
		std::vector<std::unique_ptr<TClient>> clients;

	public:
		ClientGroup(const int clientCount)
			: clientCount{clientCount}
			, ioc{std::make_unique<asio::io_context>()}
			, work_guard{asio::make_work_guard(*ioc)}
		{
		}

		~ClientGroup()
		{
			if ( workThread.joinable() )
			{
				workThread.join();
			}
			std::cout << "ClientGroup destroyed .." << std::endl;
		}

		inline void AddClientCount(const int count)
		{
			clientCount += count;
		}

		void Run(const std::string_view serverIp, const uint16_t serverPort)
		{
			for ( int i = 0; i < clientCount; ++i )
			{
				clients.emplace_back(std::make_unique<TClient>(*ioc));
				clients.back()->Connect(serverIp, serverPort);
				std::this_thread::sleep_for(1ms);
			}
			workThread = std::thread([this] ()
			{
				ioc->run();
			});
		}

		const auto& Clients() const
		{
			return clients;
		}
	};

	std::vector<std::unique_ptr<ClientGroup<TClient>>> groups;

public:

	/// <summary>
	/// 더미 클라이언트 
	/// </summary>
	/// <param name="clientCount">실행할 클라이언트 개수</param>
	/// <param name="threadCount">클라이언트 실행에 적용할 스레드 개수</param>
	void StartClients(const std::string_view serverIp, const uint16_t serverPort, const int clientCount, const int threadCount)
	{
		if ( serverIp.empty() )
		{
			std::cerr << " StartClients() -> 함수 인자 오류 : 서버의 아이피가 빈 문자열임";
			return;
		}


		if ( clientCount == 0 || threadCount == 0 || clientCount < threadCount )
		{
			std::cerr << " StartClients() -> 함수 인자 오류 : clientCount == 0 || threadCount == 0 || clientCount < threadCount";
			return;
		}

		int groupPreClientCount = clientCount / threadCount;
		int remains = clientCount % threadCount;
		for ( int i = 0; i < threadCount; ++i )
		{
			groups.emplace_back(std::make_unique<ClientGroup<TClient>>(groupPreClientCount));
		}
		groups.back()->AddClientCount(remains);

		for ( auto& group : groups )
		{
			group->Run(serverIp, serverPort);
		}
	}

	const auto& Groups() const
	{
		return groups;
	}
};