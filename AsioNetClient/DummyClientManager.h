#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <thread>
using namespace boost;

/// <summary>
/// ���� Ŭ���̾�Ʈ ����
/// </summary>
/// <typeparam name="TClient">Ŭ���̾�Ʈ ���ø�</typeparam>
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
	/// ���� Ŭ���̾�Ʈ 
	/// </summary>
	/// <param name="clientCount">������ Ŭ���̾�Ʈ ����</param>
	/// <param name="threadCount">Ŭ���̾�Ʈ ���࿡ ������ ������ ����</param>
	void StartClients(const std::string_view serverIp, const uint16_t serverPort, const int clientCount, const int threadCount)
	{
		if ( serverIp.empty() )
		{
			std::cerr << " StartClients() -> �Լ� ���� ���� : ������ �����ǰ� �� ���ڿ���";
			return;
		}


		if ( clientCount == 0 || threadCount == 0 || clientCount < threadCount )
		{
			std::cerr << " StartClients() -> �Լ� ���� ���� : clientCount == 0 || threadCount == 0 || clientCount < threadCount";
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