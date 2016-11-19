#pragma once
#include "SocketTcp.h"
#include "AsyncBase.h"

class AudioSocketReader : public Titanium::TIRA::AsyncBase
{
public:
	AudioSocketReader(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	std::function<void(void *data, unsigned int size)> OnDataReceivedEvent;
	~AudioSocketReader();
	void Start();
	void Stop();
	void Cancel();
private:
	std::shared_ptr<Titanium::TIRA::SocketTcp> m_socket;
	virtual void Workloop() override;
};

