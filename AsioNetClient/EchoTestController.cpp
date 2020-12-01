#include "EchoTestController.h"
#include <SessionManager.h>
#include <ClientControllerMap.h>
#include <PacketUtil.h>
#include <PacketDefine.h>
#include "DummyClient.h"
using namespace std::chrono;

asio::awaitable<void> EchoTestController::TestChannelEchoResFunc(TcpClient& client, msgpack::sbuffer& dataBuf)
{
	DummyClient& dumClient = dynamic_cast<DummyClient&>(client);

	auto readData = Packet::Deserialize<Packet::EchoTestReq>(dataBuf.data(), dataBuf.size());
	auto now = system_clock::now();
	system_clock::time_point writtenTime(milliseconds(readData.timestamp));
	duration<double, milli> responseMs = now - writtenTime;
	dumClient.AddEchoState(responseMs.count());
	co_return;
}

void EchoTestController::RegisterFunc()
{
	ClientControllerManager::Inst().Register(PacketId::EchoTestRes, TestChannelEchoResFunc);
}
