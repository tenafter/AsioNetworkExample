#pragma once
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include <memory>
#include "TcpSession.h"
using namespace std;
using namespace boost;
using namespace boost::asio::ip;

using SessionMap = tbb::concurrent_hash_map<uint32_t, std::shared_ptr<TcpSession>>;
using SessionIdQueue = tbb::concurrent_queue<uint32_t>;

/// <summary>
/// 서버에 접속한 세션의 관리자
/// </summary>
class SessionManager
{
private:
	SessionIdQueue sessionIdQue;
	SessionMap sessionMap;
	signals2::signal<void(size_t, TcpSession&)> onCreateSession;

public:
	static SessionManager& Inst()
	{
		static SessionManager inst;
		return inst;
	}

	SessionManager();

	std::shared_ptr<TcpSession> InsertNewSession(tcp::socket&& socket);

	bool Delete(const int sessionId);

	std::shared_ptr<TcpSession> Get(const uint32_t id) const;

	const auto Size() const
	{
		return sessionMap.size();
	}

	/// <summary>
	/// 세션이 생성될 때, 이벤트를 발생시킴
	/// boost signals2 라이브러리를 이용하여 작성됨
	/// </summary>
	/// <param name="event">이벤트 발생시 함께 보낼 데이터</param>
	/// <returns></returns>
	auto AddOnCreateSessionEvent(signals2::signal<void(size_t, TcpSession&)>::slot_type&& event)
	{
		onCreateSession.connect(event);
	}
};

