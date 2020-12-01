#include "AuthContoroller.h"
#include <iostream>
#include <SessionManager.h>
#include <ClientControllerMap.h>
#include <PacketUtil.h>
#include <PacketDefine.h>
#include <CommonConstants.h>
#include "DummyClient.h"

asio::awaitable<void> AuthController::AuthRes(TcpClient& client, msgpack::sbuffer& dataBuf)
{
	DummyClient& dumClient = dynamic_cast<DummyClient&>(client);
	auto readData = Packet::Deserialize<Packet::AuthRes>(dataBuf.data(), dataBuf.size());
	if ( readData.zoneId > ZONE_ID_NONE )
	{
		dumClient.SetZoneId(readData.zoneId);
	}
	else
	{
		std::cout << "���� ���� ���� \n";
	}
	co_return;
}

void AuthController::RegisterFunc()
{
	ClientControllerManager::Inst().Register(PacketId::AuthRes, AuthController::AuthRes);
}
