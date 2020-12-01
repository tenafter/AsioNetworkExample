#include <iostream>
#include "TcpClient.h"
#include "ClientControllerMap.h"

void TcpClient::Connect()
{
	asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 43234);
	socket.async_connect(endpoint,
		[this] (const boost::system::error_code& error)
	{
		if ( error )
		{
			std::cout << "connect failed : " << error.message() << std::endl;
			return;
		}
		asio::co_spawn(socket.get_executor(), ReadAsyncWaitLoop(), asio::detached);
		asio::co_spawn(socket.get_executor(), MainLoop(), asio::detached);
	}
	);
}

void TcpClient::Close()
{
	if ( socket.is_open() == true )
	{
		socket.close();
	}
}

asio::awaitable<void> TcpClient::ReadAsyncWaitLoop()
{
	while ( socket.is_open() == true )
	{
		readBuf.fill(0);
		try
		{
			std::size_t bytesTransferred = co_await socket.async_read_some(
				asio::buffer(readBuf.data(), PACKET_SIZE_MAX), asio::use_awaitable);
			auto [sucess, PacketType] = packetParser.PacketParse(readBuf, bytesTransferred);
			if ( sucess == true )
			{
				co_await Dispatch(PacketType, packetParser.PacketData());
			}
			readBuf.fill(0);
		}
		catch ( std::exception& ex )
		{
			std::cerr << "Client Network Error: " << ex.what() << std::endl;
			Close();
		}
		std::this_thread::yield();
	}
}

asio::awaitable<void> TcpClient::Dispatch(const PacketIdType packetId, msgpack::sbuffer& dataBuf)
{
	try
	{
		auto& controllers = ClientControllerManager::Inst();
		ControllerFunc controller
			= controllers.Func(packetId);

		if ( controller == nullptr )
		{
			std::cout << "Unknown PacketType : " << packetId << std::endl;
			co_return;
		}

		co_await controller(*this, dataBuf);
	}
	catch ( std::exception& ex )
	{
		std::cout << "Protocol Controller Error : " << ex.what() << std::endl;
		Close();
	}
}

asio::awaitable<void> TcpClient::WriteBufferAsync(const char* packetBuf, size_t packetBufSize)
{
	if ( socket.is_open() == false )
	{
		co_return;
	}
	co_await asio::async_write(
		socket, asio::buffer(packetBuf, packetBufSize),
		asio::use_awaitable
	);
}
