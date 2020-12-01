#pragma once
#include "Constants.h"
#include "Packet.h"
#include "Channel.h"
#include <unordered_map>
#include <functional>
#include <iostream>
#include "TcpSession.h"
#include <boost/asio.hpp>

//inline boost::asio::awaitable<void> authReq(Proto::IPacketData&& packetData) 
//{
//	auto data = static_cast<Proto::AuthReqPacket*>(&packetData);
//	data->userKey;
//}

using ControllerMap = std::unordered_map< PacketType, std::function<boost::asio::awaitable<void>(Proto::IPacketData&&)>>;

class TcpSession;
class Dispatcher
{
private:
	ControllerMap controllerMap;
	TcpSession* session;
public:
	Dispatcher(TcpSession* session) : session(session) {}
	void Dispatch(PacketType PacketType, Proto::IPacketData&& data);
};


