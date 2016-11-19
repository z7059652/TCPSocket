#pragma once
#include "AsyncBase.h"
#include "TcpListener.h"
#include "timebased_worker.h"
#include "RequestMsg.h"
#include "ResponseMsg.h"
class RtspSession : public Titanium::TIRA::AsyncBase
{
public:
	RtspSession(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	~RtspSession();
	void Start();
	void Stop();
	std::function<void(const RequestMsg &msg)> ReceivedRequestEvent;
	std::function<void(const ResponseMsg &msg)> ReceivedResponseEvent;
	std::function<void()> DisconnectedByClientEvent;
	void Send(IMsg& msg);
private:
	void OnDisconnect();
	virtual void Workloop() override;
	void OnReceive(const IMsg& msg);
	void HandleMessage(const IMsg& msg);
	std::shared_ptr<Titanium::TIRA::SocketTcp> m_Socket;
	std::string m_Buf;
	void OnReceiveRequest(const RequestMsg& request);
	void OnReceiveResponse(const ResponseMsg& response);


	Titanium::TIRA::TimeBasedWorker m_HeartBeatTimer;
	Titanium::TIRA::TimeBasedWorker m_CheckAliveTimer;
	std::atomic<bool> m_IsAlive;
	std::mutex m_Mtx;
	int m_CSeq;
};