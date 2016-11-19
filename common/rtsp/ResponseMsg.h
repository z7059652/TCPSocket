#pragma once
#include "IMsg.h"

class ResponseMsg: public IMsg
{
public:
	ResponseMsg(int cseq, bool result, const std::string &data = "");
	ResponseMsg(const IMsg &msg);
	virtual ~ResponseMsg();
	bool GetResult() const;
};