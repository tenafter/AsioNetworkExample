#pragma once
#include <unordered_map>
#include <boost/asio.hpp>
using namespace boost;

/// <summary>
/// 서버 사이드 패킷 컨트롤러 함수 정의
/// </summary>
using ControllerFunc = std::function<asio::awaitable<void>(const uint32_t sessionId, msgpack::sbuffer&)>;
/// <summary>
/// 패킷아이디와 패킷 컨트롤러 함수 맵
/// </summary>
using ControllerMap = std::unordered_map<PacketIdType, ControllerFunc>;

/// <summary>
/// 패킷 컨트롤러 관리자
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