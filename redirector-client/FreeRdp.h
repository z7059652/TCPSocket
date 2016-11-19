#pragma once
#include <libfreerdp/core/rdp.h>

class FreeRdp
{
protected:
	rdpContext* context;
public:
	FreeRdp();
	int start(char* para);
	~FreeRdp();
	int stop();
};

