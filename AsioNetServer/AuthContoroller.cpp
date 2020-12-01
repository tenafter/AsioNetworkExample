#include "AuthContoroller.h"
#include <SessionManager.h>
#include <ServerControllerMap.h>
#include <PacketUtil.h>
#include <PacketDefine.h>
#include "AuthEmulator.h"

asio::awaitable<void> AuthController::AuthReq(const uint32_t sessionId, msgpack::sbuffer& dataBuf)
{
	auto session = SessionManager::Inst().Get(sessionId);
	if ( session == nullptr ) co_return;

	auto data = Packet::Deserialize<Packet::AuthReq>(dataBuf.data(), dataBuf.size());

	uint64_t zoneId = co_await AuthEmulator::Inst().Authorize(data.accessToken);
	Packet::AuthRes writePacket;
	writePacket.zoneId = zoneId;
	co_await session->WriteAsync<Packet::AuthRes>(std::move(writePacket));
}

void AuthController::RegisterFunc()
{
	ControllerManager::Inst().Register(PacketId::AuthReq, AuthController::AuthReq);
}