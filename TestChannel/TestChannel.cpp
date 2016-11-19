// TestChannel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TcpChannel.h"
using namespace Titanium;
using namespace Titanium::TIRA;

int _tmain(int argc, _TCHAR* argv[])
{
	SocketFuncs::Startup();
	InetAddress addr("10.172.98.205", 43221);
	auto msock = SocketTcp::ConnectTo(addr);
	msock->SetTcpNoDelay(true);
	msock->SetRecvBufferSize(6);
	msock->SetSendBufferSize(5);

	auto videoChannel = std::make_shared<Titanium::TcpChannel>(msock);
	videoChannel->RegisterDataReceivedEvent([&](const unsigned char *data, int size)
	{
		char* p = "Hello World";
		videoChannel->Send((const unsigned char *)p, 6);
//		msock->Send((const unsigned char *)p, 6);
	});
	videoChannel->Start();
	while (true)
	{

	}
	return 0;
}

