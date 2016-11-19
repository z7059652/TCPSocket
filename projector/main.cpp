#include "Session.h"
#include "SocketTcpServer.h"
#include "SessionManager.h"
#include <thread>
#define COMMAND_SERVER_PORT 5262

using namespace std;
using namespace Titanium;
using namespace Titanium::TIRA;

int main(int argc, char* argv[])
{
	SocketFuncs::Startup();
	SessionManager sessionManager;

	auto serverSocket = make_shared<SocketTcpServer>(COMMAND_SERVER_PORT);
	serverSocket->StartListening();
	unsigned int tm = 0;
	while (true)
	{
		auto socket = serverSocket->WaitForConnection();
		auto session = make_shared<Session>(socket);
		session->ConnectedEvent = [&](Session* s)
		{
			sessionManager.SetActive(s);
		};
		session->DisconnectedEvent = [&](Session* s)
		{
			thread([&]() { sessionManager.Remove(s); }).detach();
		};
		session->Start();
		sessionManager.Add(session);
	}
	return 0;
}