#pragma once
enum STATUS
{
	Run = 1,
	Stop = 2
};

class IAgent
{
public:
	IAgent();
	virtual ~IAgent();
	virtual void Disconnect();
};

