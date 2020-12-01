#pragma once
#include <unordered_map>
#include <boost/asio.hpp>
using namespace boost;

/// <summary>
/// 클라이언트 패킷 컨트롤러 함수 형식 정의
/// </summary>
using ControllerFunc = std::function<asio::awaitable<void>(TcpClient&, msgpack::sbuffer&)>;
/// <summary>
/// 클라이언트 함수의 맵 정의
/// </summary>
using ControllerMap = std::unordered_map<PacketIdType, ControllerFunc>;

/// <summary>
/// 테스트용 클라이언트의 패킷 컨트롤러 관리자
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
	/// 패킷 컨트롤러 등록
	/// </summary>
	/// <param name="packetId">패킷아이디</param>
	/// <param name="func">컨트롤러 함수</param>
	void Register(PacketIdType packetId, ControllerFunc func)
	{
		controllerMap[packetId] = func;
	}

	/// <summary>
	/// 패킷 컨트롤러 가져오기
	/// </summary>
	/// <param name="packetId"></param>
	/// <returns></returns>
	inline auto Func(const PacketIdType packetId)
	{
		return controllerMap[packetId];
	}

};