#pragma once
#include "string"
#include <sstream>

#define RTSP_HEAD_DATA "Data"
#define RTSP_HEAD_TYPE "Type"
#define RTSP_HEAD_CSEQ "CSeq"
#define RTSP_HEAD_CMD "Cmd"

#define RTSP_TYPE_REQ "Req"
#define RTSP_TYPE_RESP "Resp"
#define RTSP_TYPE_PARAM "Param"
#define RTSP_TYPE_HEART_BEAT "HEARTBEAT"

#define RTSP_RESPONSE_OK " 200 OK"
#define RTSP_RESPONSE_FAIL " 100 FAIL"

#define RTSP_MSG_SEPARATOR "\r\n"
#define RTSP_MSG_END "\r\n\r\n"
#define RTSP_MSG_MAX_SIZE 256

class IMsg
{
public:
	IMsg();
	IMsg(const std::string &str);
	virtual ~IMsg();

	virtual std::string GetType() const;
	virtual std::string GetData() const;
	virtual int GetCSeq() const;
	virtual std::string GetCSeqString() const;
	virtual std::string GetCmd() const;
	virtual void SetCSeq(int cseq);
	virtual std::string ToString() const;
protected:
	std::string _type;
	std::string _cmd;
	std::string _data;
	int _cseq;
};

