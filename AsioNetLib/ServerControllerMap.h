#pragma once
#include <unordered_map>
#include <boost/asio.hpp>
using namespace boost;

/// <summary>
/// ���� ���̵� ��Ŷ ��Ʈ�ѷ� �Լ� ����
/// </summary>
using ControllerFunc = std::function<asio::awaitable<void>(const uint32_t sessionId, msgpack::sbuffer&)>;
/// <summary>
/// ��Ŷ���̵�� ��Ŷ ��Ʈ�ѷ� �Լ� ��
/// </summary>
using ControllerMap = std::unordered_map<PacketIdType, ControllerFunc>;

/// <summary>
/// ��Ŷ ��Ʈ�ѷ� ������
/// </summary>
class ControllerManager
{
private:
	ControllerMap controllerMap;

public:
	static ControllerManager& Inst()
	{
		static ControllerManager inst;
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