#pragma once
#include "IMsg.h"

class RequestMsg : public IMsg
{
public:
	RequestMsg(const std::string &request);
	RequestMsg(const IMsg &msg);
	virtual ~RequestMsg();

};