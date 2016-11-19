#pragma once
#include <list>
#include <memory>
#include <mutex>

class Session;
class SessionManager
{
public:
	void Add(std::shared_ptr<Session> session);
	void Remove(Session *session);
	void SetActive(Session *session);
private:
	std::list<std::shared_ptr<Session>> _sessions;
	Session *_activeSession;
	std::mutex _mutex;
};