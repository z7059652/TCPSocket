#pragma once
struct RdpStreamingMsgHeader
{
	unsigned int  m_PayloadLength;
};

struct RdpStreamingMessage
{
	RdpStreamingMsgHeader m_ObjectHeader;
	unsigned char*    m_ObjectBody;
};

const unsigned int g_MsgHeaderSize = sizeof(RdpStreamingMsgHeader);
const unsigned int g_MessageSize = sizeof(RdpStreamingMessage);

