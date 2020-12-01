#include "EchoTestController.h"
#include <SessionManager.h>
#include <ServerControllerMap.h>
#include <PacketUtil.h>
#include <PacketDefine.h>
#include <IocChannelManager.h>
#include <iostream>

asio::awaitable<void> EchoTestController::EchoTestReq(
	const uint32_t sessionId, msgpack::sbuffer& dataBuf)
{
	auto session = SessionManager::Inst().Get(sessionId);
	if ( session == nullptr ) co_return;
	auto data = Packet::Deserialize<Packet::EchoTestReq>(dataBuf.data(), dataBuf.size());

	auto& channel = IocChannelManager::Inst().GetChannel(sessionId);
	auto& ioc = channel.IocExecutor();
	co_await asio::co_spawn(ioc, [data, session] () -> asio::awaitable<void>
	{
		Packet::EchoTestRes writePacket;
		writePacket.seqId = data.seqId;
		writePacket.testString = data.testString;
		std::memcpy(writePacket.bin.data(), data.bin.data(), data.bin.size());
		writePacket.timestamp = data.timestamp;
		co_await session->WriteAsync<Packet::EchoTestRes>(std::move(writePacket));
	}
	, asio::use_awaitable);
}

void EchoTestController::RegisterFunc()
{
	ControllerManager::Inst().Register(PacketId::EchoTestReq, EchoTestController::EchoTestReq);
}
