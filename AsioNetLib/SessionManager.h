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
/// ������ ������ ������ ������
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
	/// ������ ������ ��, �̺�Ʈ�� �߻���Ŵ
	/// boost signals2 ���̺귯���� �̿��Ͽ� �ۼ���
	/// </summary>
	/// <param name="event">�̺�Ʈ �߻��� �Բ� ���� ������</param>
	/// <returns></returns>
	auto AddOnCreateSessionEvent(signals2::signal<void(size_t, TcpSession&)>::slot_type&& event)
	{
		onCreateSession.connect(event);
	}
};

