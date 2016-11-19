#include "FreeRdp.h"
#include "memory"
#include "Redirector.h"
#include "ProjectorSetting.h"
#include "Socket.h"
#include <exception>
using namespace std;
using namespace Titanium::TIRA;

int main(int argc, char* argv[])
{
	SocketFuncs::Startup();
	shared_ptr<Redirector> pRedirector = make_shared<Redirector>();

	RdpSetting rdpSetting = { "", "", "", "" };
	ProjectorSetting projectorSetting = { "", 5262, 5263 };

	try
	{
		pRedirector->Start(projectorSetting, rdpSetting);
	}
	catch (std::exception &e)
	{
		OutputDebugStringA(e.what());
	}
	while (true)
	{
		Sleep(200);
	}
	return 0;
}

