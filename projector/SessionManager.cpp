#include "SessionManager.h"
#include "Session.h"

using namespace std;

void SessionManager::Add(shared_ptr<Session> session)
{
	lock_guard<mutex> lock(_mutex);
	_sessions.push_back(session);
}
void SessionManager::Remove(Session *session)
{
	lock_guard<mutex> lock(_mutex);
	if (session == _activeSession)
		_activeSession = nullptr;
	_sessions.remove_if([=](shared_ptr<Session> s){return session == s.get(); });
}
void SessionManager::SetActive(Session *session)
{
	lock_guard<mutex> lock(_mutex);
	if (_activeSession != nullptr)
		Remove(_activeSession);
	_activeSession = session;
}