#include "RequestMsg.h"
#include <sstream>
using namespace std;

RequestMsg::RequestMsg(const std::string &request)
{
	_type = RTSP_TYPE_REQ;
	_cmd = request;
}

RequestMsg::RequestMsg(const IMsg &msg)
	: IMsg(msg)
{
}

RequestMsg::~RequestMsg()
{
}

