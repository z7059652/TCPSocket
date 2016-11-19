#pragma once
#include "TcpChannel.h"

class TouchAndPen
{
public:
	TouchAndPen(std::shared_ptr<Titanium::TcpChannel> channel);
	~TouchAndPen();
	void Send(POINTER_INFO *pointers, int count);
private:
	std::shared_ptr<Titanium::TcpChannel> _channel;
	int lastFrameId;
};

