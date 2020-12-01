#pragma once
#include <memory>
#include <boost/asio.hpp>
using namespace boost;
using namespace boost::asio::ip;

/// <summary>
/// TCP 서버 클래스
/// </summary>
class TcpServer
{
private:
	/// <summary>
	/// 클라이언트의 접속을 대기하는 클래스
	/// 개별 스레드를 생성하고 독자적인 io_context로 작업을 수행함
	/// </summary>
	class TcpAcceptor
	{
	private:

		std::unique_ptr<asio::io_context> ioc;
		asio::executor_work_guard<asio::io_context::executor_type> work_guard;
		std::thread iocThread;
		tcp::acceptor& acceptor;
		bool isInit{false};

		asio::awaitable<void> Listen(tcp::acceptor& acceptor, asio::io_context& iocForSocket);

	public:
		TcpAcceptor(tcp::acceptor& acceptor)
			: ioc{std::make_unique<asio::io_context>()}
			, work_guard{asio::make_work_guard(*ioc)}
			, acceptor{acceptor}
		{}
		~TcpAcceptor();

		void Run();

		void Stop();
	};

	std::vector<std::unique_ptr<TcpAcceptor>> workers;
	std::unique_ptr<asio::io_context> iocForAcceptor;
	std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard;
	std::unique_ptr<tcp::acceptor> acceptor;
	bool isInit{false};

public:
	virtual ~TcpServer()
	{}

	/// <summary>
	/// 서버 초기화
	/// </summary>
	/// <param name="port">서버 포트</param>
	/// <param name="ioThreadCnt">네트워크 io 스레드 수</param>
	/// <param name="channelThreadCnt">채널 io 스레드 수</param>
	void Init(const unsigned short port, const int ioThreadCnt, const int channelThreadCnt);

	void Start();

	void Stop();

	/// <summary>
	/// Tcp accept와 송수신 처리를 위한 iocontext
	/// </summary>
	/// <returns>asio::io_context</returns>
	auto& IoContext()
	{
		return *iocForAcceptor;
	}
};

