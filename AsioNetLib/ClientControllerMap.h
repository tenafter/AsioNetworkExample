#pragma once
#include <unordered_map>
#include <boost/asio.hpp>
using namespace boost;

/// <summary>
/// Ŭ���̾�Ʈ ��Ŷ ��Ʈ�ѷ� �Լ� ���� ����
/// </summary>
using ControllerFunc = std::function<asio::awaitable<void>(TcpClient&, msgpack::sbuffer&)>;
/// <summary>
/// Ŭ���̾�Ʈ �Լ��� �� ����
/// </summary>
using ControllerMap = std::unordered_map<PacketIdType, ControllerFunc>;

/// <summary>
/// �׽�Ʈ�� Ŭ���̾�Ʈ�� ��Ŷ ��Ʈ�ѷ� ������
/// </summary>
class ClientControllerManager
{
private:
	ControllerMap controllerMap;

public:
	static ClientControllerManager& Inst()
	{
		static ClientControllerManager inst;
		return inst;
	}

	/// <summary>
	/// ��Ŷ ��Ʈ�ѷ� ���
	/// </summary>
	/// <param name="packetId">��Ŷ���̵�</param>
	/// <param name="func">��Ʈ�ѷ� �Լ�</param>
	void Register(PacketIdType packetId, ControllerFunc func)
	{
		controllerMap[packetId] = func;
	}

	/// <summary>
	/// ��Ŷ ��Ʈ�ѷ� ��������
	/// </summary>
	/// <param name="packetId"></param>
	/// <returns></returns>
	inline auto Func(const PacketIdType packetId)
	{
		return controllerMap[packetId];
	}

};