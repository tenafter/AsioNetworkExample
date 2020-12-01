#pragma once
#include <msgpack.hpp>
#include "../AsioNetLib/Constants.h"

enum PacketId : PacketIdType
{
	None,
	EchoTestReq,
	EchoTestRes,

	AuthReq,
	AuthRes,

	ZoneJoinReq,
	ZoneJoinRes,

	ZoneLeaveReq,
	ZoneLeaveRes,

	ZoneServerNoti,
};

