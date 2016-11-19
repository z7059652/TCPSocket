
#pragma once
#include "SocketTcp.h"
#include "AsyncBase.h"


struct _RDPGFX_FRAME_ACKNOWLEDGE_PDU
{
	UINT32 queueDepth;
	UINT32 frameId;
	UINT32 totalFramesDecoded;
};
typedef struct _RDPGFX_FRAME_ACKNOWLEDGE_PDU RDPGFX_FRAME_ACKNOWLEDGE_PDU;

class ScreenDataChannel : public Titanium::TIRA::AsyncBase
{
public:
	ScreenDataChannel(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	std::function<void(RDPGFX_FRAME_ACKNOWLEDGE_PDU* pScreeData)> OnDataReceivedEvent;
	~ScreenDataChannel();
	void Start();
	void Stop();
	void Cancel();
	void Send(void *data, unsigned int size);
private:
	std::shared_ptr<Titanium::TIRA::SocketTcp> m_socket;
	virtual void Workloop() override;
};

