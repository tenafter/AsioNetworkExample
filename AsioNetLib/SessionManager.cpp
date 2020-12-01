#include "SessionManager.h"
#include "Constants.h"

SessionManager::SessionManager()
{
	for ( uint32_t i = 1; i < SESSION_COUNT_MAX * 2; ++i )
	{
		sessionIdQue.push(i);
	}
}

std::shared_ptr<TcpSession> SessionManager::InsertNewSession(boost::asio::ip::tcp::socket&& socket)
{
	uint32_t sessionId = 0;
	if ( sessionIdQue.try_pop(sessionId) == false )
	{
		// sessionIdQue에 더이상 값이 없으므로 세션을 끊고 재접속하도록 유도한다.
		// 에러가 발생한다면 sessionIdQue에 미리 넣어둔 값이 부족하지 않은지 체크할 것
		return nullptr;
	}

	auto session = std::make_shared<TcpSession>(std::move(socket), sessionId);
	if ( sessionMap.insert({sessionId, session}) == false )
	{
		// 세션 매니저에 등록이 불가한 상태
		sessionIdQue.push(sessionId);
		return nullptr;
	}

	session->AddOnStopEvent([this] (const int sessionId)
	{
		Delete(sessionId);
	});
	onCreateSession(Size(), *session);

	return session;
}

bool SessionManager::Delete(const int sessionId)
{
	if ( sessionMap.erase(sessionId) == true )
	{
		sessionIdQue.push(sessionId);
	}
	return true;
}

std::shared_ptr<TcpSession> SessionManager::Get(const uint32_t id) const
{
	SessionMap::accessor result;
	bool findOk = sessionMap.find(result, id);
	if ( findOk )
	{
		return result->second;
	}
	return nullptr;
}
