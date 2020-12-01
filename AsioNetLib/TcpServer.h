#pragma once
#include <memory>
#include <boost/asio.hpp>
using namespace boost;
using namespace boost::asio::ip;

/// <summary>
/// TCP ���� Ŭ����
/// </summary>
class TcpServer
{
private:
	/// <summary>
	/// Ŭ���̾�Ʈ�� ������ ����ϴ� Ŭ����
	/// ���� �����带 �����ϰ� �������� io_context�� �۾��� ������
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
	/// ���� �ʱ�ȭ
	/// </summary>
	/// <param name="port">���� ��Ʈ</param>
	/// <param name="ioThreadCnt">��Ʈ��ũ io ������ ��</param>
	/// <param name="channelThreadCnt">ä�� io ������ ��</param>
	void Init(const unsigned short port, const int ioThreadCnt, const int channelThreadCnt);

	void Start();

	void Stop();

	/// <summary>
	/// Tcp accept�� �ۼ��� ó���� ���� iocontext
	/// </summary>
	/// <returns>asio::io_context</returns>
	auto& IoContext()
	{
		return *iocForAcceptor;
	}
};

