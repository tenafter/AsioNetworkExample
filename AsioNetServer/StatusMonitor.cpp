#include "StatusMonitor.h"

asio::awaitable<void> StatusMonitor::ShowLoop(asio::io_context& ioc)
{
	asio::steady_timer timer(ioc);
	while ( ioc.stopped() == false )
	{
		Show();
		timer.expires_from_now(interval);
		system::error_code ec;
		co_await timer.async_wait(asio::redirect_error(asio::use_awaitable, ec));
		if ( ec )
		{
			std::cout << ec.message() << std::endl;
			break;
		}
	}
}

void StatusMonitor::Show()
{
	auto now = system_clock::now();
	if ( show_title++ % 10 == 0 )
	{
		printf("\n");
		printf("networkThread:%d, channelThread:%d\n", networkThreadCount, channelThreadCount);
		printf("connections    read count    write count    time delta(ms)\n");
	}
	printf("%5lld        %7lld        %7lld        %7lld\n",
		connections,
		totalReadCount - lastReadCount,
		totalWriteCount - lastWriteCount,
		duration_cast<milliseconds>(now - last_time).count()
	);

	last_time = now;
	lastReadCount.store(totalReadCount);
	lastWriteCount.store(totalWriteCount);
}

void StatusMonitor::Start(asio::io_context& ioc, int networkThreadCount, int channelThreadCount)
{
	this->networkThreadCount = networkThreadCount;
	this->channelThreadCount = channelThreadCount;

	SessionManager::Inst().AddOnCreateSessionEvent(
		[this] (size_t sessionCount, TcpSession& session)
	{
		connections = sessionCount;

		session.AddOnReadEvent([this] (size_t bytesTransferred)
		{
			totalReadCount.fetch_add(1);
			totalReadSize.fetch_add(bytesTransferred);
		});

		session.AddOnWriteEvent([this] (size_t bytesTransferred)
		{
			totalWriteCount.fetch_add(1);
			totalWriteSize.fetch_add(bytesTransferred);
		});

		session.AddOnStopEvent([this] (int sessionId)
		{
			connections = SessionManager::Inst().Size();
		});
	});

	asio::co_spawn(ioc, ShowLoop(ioc), asio::detached);
}
