#pragma once
#include "SocketTcp.h"
#include "AsyncBase.h"


typedef struct HIDDATA {
	enum HIDType type;
	UINT Msg;
	UINT xPos;
	UINT yPos;
} HIDDATASTRUCT;
class HIDSocketReader : public Titanium::TIRA::AsyncBase
{
public:
	HIDSocketReader(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	std::function<void(const HIDDATASTRUCT* pHidData)> OnDataReceivedEvent;
	~HIDSocketReader();
	void Start();
	void Stop();
	void Cancel();
private:
	std::shared_ptr<Titanium::TIRA::SocketTcp> m_socket;
	virtual void Workloop() override;
};

