#include "ResponseMsg.h"


ResponseMsg::ResponseMsg(int cseq, bool result, const std::string &data)
{
	_cseq = cseq;
	_type = result ? RTSP_TYPE_RESP RTSP_RESPONSE_OK : RTSP_TYPE_RESP RTSP_RESPONSE_FAIL;
	_data = data;
}

ResponseMsg::ResponseMsg(const IMsg &msg)
	: IMsg(msg)
{
}

ResponseMsg::~ResponseMsg()
{
}

bool ResponseMsg::GetResult() const
{
	return _type == RTSP_TYPE_RESP RTSP_RESPONSE_OK;
}