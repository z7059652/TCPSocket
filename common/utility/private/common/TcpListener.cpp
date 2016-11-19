#include "TcpListener.h"

using namespace Titanium::TIRA;
namespace Titanium
{

	TcpListener::TcpListener(unsigned int port)
		: AsyncBase("TcpListener")
		, m_Port(port)
	{
	}
	TcpListener::~TcpListener(void)
	{
		Stop();
	}
	void TcpListener::Start()
	{
		m_pServer = std::make_shared<SocketTcpServer>(m_Port);
		m_pServer->StartListening();
		AsyncBase::StartAsyncBase();
	}
	SignalConnection TcpListener::RegisterConnectionAcceptedEvent(OnConnectionAcceptedAction action)
	{
		return std::move(m_SigConnected.connect(action));
	}
	void TcpListener::Stop()
	{
		AsyncBase::StopAsyncBase();
	}
	void TcpListener::Workloop()
	{
		while(CanLoopContinue()) 
		{			
			auto connection = m_pServer->WaitForConnection();
			m_SigConnected(connection);			
		}
	}
	void TcpListener::UnBlockWorkingThread()
	{
		m_pServer->StopListening();
	}
}