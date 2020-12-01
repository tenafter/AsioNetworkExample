#include "TcpSession.h"
#include "SessionManager.h"
#include "IocChannelManager.h"
#include "ServerControllerMap.h"
#include "Constants.h"

asio::awaitable<void> TcpSession::ReadAsyncWaitLoop()
{
	try
	{
		while ( socket.is_open() == true )
		{
			std::size_t bytesTransferred = co_await socket.async_read_some(
				boost::asio::buffer(readBuf.data(), PACKET_SIZE_MAX),
				asio::use_awaitable);

			onRead(bytesTransferred);

			auto [sucess, PacketType] = packetParser.PacketParse(readBuf, bytesTransferred);
			if ( sucess == true )
			{
				co_await Dispatch(PacketType, packetParser.PacketData());
			}
			readBuf.fill(0);
		}
	}
	catch ( std::exception& ex )
	{
		// 소켓의 예외발생 - 연결종료 등
		Stop();
	}
}

asio::awaitable<void> TcpSession::Dispatch(const PacketIdType packetId, msgpack::sbuffer& dataBuf)
{
	try
	{
		auto& controllers = ControllerManager::Inst();
		ControllerFunc controller
			= controllers.Func(packetId);

		if ( controller == nullptr )
		{
			std::cout << "Unknown PacketType : " << packetId << std::endl;
			co_return;
		}

		co_await controller(sessionId, dataBuf);

	}
	catch ( std::exception& ex )
	{
		std::cout << "Protocol Controller Error : " << ex.what() << std::endl;
		Stop();
	}
}

void TcpSession::Stop()
{
	if ( socket.is_open() == true )
	{
		try
		{
			system::error_code error;
			socket.shutdown(tcp::socket::shutdown_both, error);
			socket.close(error);
		}
		catch ( system::system_error& e )
		{
			std::cout << "[Network error - closeSocket] " << e.what() << std::endl;
		}
	}
	onStop(sessionId);
	timer.cancel();
}

