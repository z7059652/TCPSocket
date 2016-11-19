#include "RtspSession.h"
#include "thread.h"
#include <sstream>
using namespace Titanium::TIRA;
using namespace std;

RtspSession::RtspSession(std::shared_ptr<SocketTcp> socket)
	: AsyncBase("RtspSession")
	, m_Socket(socket)
	, m_HeartBeatTimer(5000, "rtsp server heart beat")
	, m_CheckAliveTimer(10000, "rtsp server watch dog")
	, m_IsAlive(true)
	, m_CSeq(0)
{
}
RtspSession::~RtspSession()
{
	Stop();
}
void RtspSession::Start()
{
	m_Buf = "";
	m_HeartBeatTimer.RegisterCallback([&](void*)
	{
		RequestMsg request(RTSP_TYPE_HEART_BEAT);
		request.SetCSeq(m_CSeq++);
		try
		{
			Send(request);
		}
		catch (...)
		{

		}
	});
	m_HeartBeatTimer.Start();
	m_CheckAliveTimer.RegisterCallback([&](void*)
	{
#ifndef _DEBUG
		if (m_IsAlive)
			m_IsAlive = false;
		else
			DisconnectedByClientEvent();
#endif
	});
	m_CheckAliveTimer.Start();
	StartAsyncBase();
}

void RtspSession::OnReceive(const IMsg& msg)
{
	if (msg.GetType() == RTSP_TYPE_REQ)
		OnReceiveRequest(RequestMsg(msg));
	else if (msg.GetType().find(RTSP_TYPE_RESP) != msg.GetType().npos)
		OnReceiveResponse(ResponseMsg(msg));
	else
		throw ExceptionWithString("error message");
}

void RtspSession::OnDisconnect()
{
	if (DisconnectedByClientEvent)
		DisconnectedByClientEvent();
}

void RtspSession::Stop()
{
	m_HeartBeatTimer.Stop();
	m_CheckAliveTimer.Stop();
	m_Socket->Shutdown();
	StopAsyncBase();
}

void RtspSession::Workloop()
{
	char buf[RTSP_MSG_MAX_SIZE];
	while (CanLoopContinue())
	{
		int len = 0;
		try
		{
			len = m_Socket->Recv((unsigned char*)buf, RTSP_MSG_MAX_SIZE);
		}
		catch (ExceptionWithString)
		{
			OnDisconnect();
			break;
		}
		m_Buf.append(buf, len);
		int endPos = m_Buf.find(RTSP_MSG_END);
		if (endPos != std::string::npos)
		{
			std::string msg = m_Buf.substr(0, endPos + strlen(RTSP_MSG_END));
			m_Buf = m_Buf.substr(endPos + strlen(RTSP_MSG_END));
			HandleMessage(IMsg(msg));
		}
	}
}

void RtspSession::HandleMessage(const IMsg& msg)
{
	OnReceive(msg);
	m_IsAlive = true;
}

void RtspSession::Send(IMsg& msg)
{
	std::lock_guard<std::mutex> lock(m_Mtx);
	if (msg.GetType() == RTSP_TYPE_REQ)
	{
		msg.SetCSeq(m_CSeq++);
	}
	string data = msg.ToString();
	m_Socket->Send((const unsigned char*)data.c_str(), data.length());
}

void RtspSession::OnReceiveResponse(const ResponseMsg& response)
{
	if (ReceivedResponseEvent)
		ReceivedResponseEvent(response);
}

void RtspSession::OnReceiveRequest(const RequestMsg& request)
{
	if (request.GetCmd() == RTSP_TYPE_HEART_BEAT)
		Send(ResponseMsg(request.GetCSeq(), true));
	else if (ReceivedRequestEvent)
		ReceivedRequestEvent(request);
	else
		throw ExceptionWithString("Type error");
}
