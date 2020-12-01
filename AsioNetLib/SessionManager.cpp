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
		// sessionIdQue�� ���̻� ���� �����Ƿ� ������ ���� �������ϵ��� �����Ѵ�.
		// ������ �߻��Ѵٸ� sessionIdQue�� �̸� �־�� ���� �������� ������ üũ�� ��
		return nullptr;
	}

	auto session = std::make_shared<TcpSession>(std::move(socket), sessionId);
	if ( sessionMap.insert({sessionId, session}) == false )
	{
		// ���� �Ŵ����� ����� �Ұ��� ����
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
