#include <iostream>

#include "TcpServer.h"
#include "TcpSession.h"
#include "SessionManager.h"
#include "IocChannelManager.h"


inline TcpServer::TcpAcceptor::~TcpAcceptor()
{
	Stop();
}

void TcpServer::TcpAcceptor::Run()
{
	iocThread = std::thread
	{
		[this]()
	{
		asio::co_spawn(acceptor.get_executor(), Listen(acceptor, *ioc), asio::detached);
		try
		{
			ioc->run();
		}
		catch ( std::exception& ex )
		{
			std::cerr << ex.what() << "\n";
		}
	}
	};
}

inline void TcpServer::TcpAcceptor::Stop()
{
	ioc->stop();
	if ( iocThread.joinable() )
	{
		iocThread.join();
	}
}

asio::awaitable<void> TcpServer::TcpAcceptor::Listen(tcp::acceptor& acceptor, asio::io_context& iocForSocket)
{
	while ( true )
	{
		tcp::socket socket(iocForSocket);
		co_await acceptor.async_accept(socket, asio::use_awaitable);
		auto session = SessionManager::Inst().InsertNewSession(std::move(socket));
		if ( session == nullptr )
		{
			session->Stop();
		}
		else
		{
			session->Start();
		}
		std::this_thread::yield();
	}
}

//////////////////////////////////

void TcpServer::Init(const unsigned short port, const int ioThreadCnt, const int channelThreadCnt)
{
	if ( isInit == false )
	{
		iocForAcceptor.reset(new asio::io_context(ioThreadCnt));
		work_guard.reset(
			new asio::executor_work_guard<asio::io_context::executor_type>(
				asio::make_work_guard(*iocForAcceptor)));
		acceptor.reset(new tcp::acceptor(*iocForAcceptor, {tcp::v4(), port}));

		for ( int i = 0; i < ioThreadCnt; ++i )
		{
			workers.emplace_back(std::make_unique<TcpAcceptor>(*acceptor));
		}

		IocChannelManager::Inst().Init(channelThreadCnt);
	}
}

void TcpServer::Start()
{
	if ( iocForAcceptor != nullptr )
	{
		for ( auto& worker : workers )
		{
			worker->Run();
		}
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		uint64_t millis = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
		std::cout << "Server Started : " << millis << std::endl;
		iocForAcceptor->run();
	}
	else
	{
		std::cout << "Init()으로 먼저 초기화를 진행한 후 사용바람" << std::endl;
	}
}

void TcpServer::Stop()
{
	if ( iocForAcceptor != nullptr )
	{
		iocForAcceptor->stop();
		for ( auto& worker : workers )
		{
			worker->Stop();
		}
	}
	else
	{
		std::cout << "Init()으로 먼저 초기화를 진행한 후 사용바람" << std::endl;
	}
}