#pragma once
#include "SocketTcp.h"
#include "AsyncBase.h"
#ifdef _WINRT_DLL
#include "MouseTouchPenMsg.h"
#endif

class TouchAndPenMsgReader : public Titanium::TIRA::AsyncBase
{
public:
	TouchAndPenMsgReader(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	std::function<void(POINTER_INFO*, int)> OnTouchFrameReceivedEvent;
	std::function<void(POINTER_PEN_INFO*)> OnPenFrameReceivedEvent;
	~TouchAndPenMsgReader();
	void Start();
	void Stop();
	void Cancel();
private:
	std::shared_ptr<Titanium::TIRA::SocketTcp> m_socket;
	virtual void Workloop() override;
};

