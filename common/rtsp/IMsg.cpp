#include "IMsg.h"
#include <sstream>
using namespace std;

static string DeriveString(const string &token, const string &str, const string &begin, const string &end)
{
	string target = token + begin;

	int startPos = str.find(target);
	if (startPos == string::npos)
		return "";
	startPos += target.length();

	int endPos = str.find(end, startPos);
	if (endPos == string::npos)
		return "";
	return move(str.substr(startPos, endPos - startPos));
}


IMsg::IMsg()
	: _cseq(-1)
{
}
IMsg::IMsg(const std::string & str)
{
	_data = DeriveString(RTSP_HEAD_DATA, str, ":", RTSP_MSG_SEPARATOR);
	_type = DeriveString(RTSP_HEAD_TYPE, str, ":", RTSP_MSG_SEPARATOR);
	_cmd = DeriveString(RTSP_HEAD_CMD, str, ":", RTSP_MSG_SEPARATOR);
	string cseq = DeriveString(RTSP_HEAD_CSEQ, str, ":", RTSP_MSG_SEPARATOR);
	istringstream is(cseq);
	is >> _cseq;
}
IMsg::~IMsg()
{
}
int IMsg::GetCSeq() const
{
	return _cseq;
}

std::string IMsg::GetCSeqString() const
{
	ostringstream os;
	os << _cseq;
	return os.str();
}

std::string IMsg::GetCmd() const
{
	return _cmd;
}

void IMsg::SetCSeq(int cseq)
{
	_cseq = cseq;
}

std::string IMsg::GetType() const
{
	return _type;
}

std::string IMsg::GetData() const
{
	return _data;
}

string IMsg::ToString() const
{
	ostringstream os;
	os << RTSP_HEAD_TYPE":" << _type << RTSP_MSG_SEPARATOR;
	os << RTSP_HEAD_CSEQ":" << _cseq << RTSP_MSG_SEPARATOR;
	os << RTSP_HEAD_CMD":" << _cmd << RTSP_MSG_SEPARATOR;
	os << RTSP_HEAD_DATA":" << _data << RTSP_MSG_END;
	return os.str();
}