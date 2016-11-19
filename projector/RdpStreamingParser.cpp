#include "RdpStreamingParser.h"
#include "RdpStreamingDef.h"

RdpStreamingParser::RdpStreamingParser() :
m_Stage(ParseStage::Header)
{
	m_HeaderBuffer.SetBufferSizeLowerBound(g_MessageSize);

	//TODO: here is a potential bug regarding big endian and little endian problem is not considered.
	m_pHeader = (RdpStreamingMsgHeader*)m_HeaderBuffer.GetPtrForWriting();
	m_pMessage = (RdpStreamingMessage*)m_HeaderBuffer.GetPtrForWriting();
}
void RdpStreamingParser::Process(const unsigned char* pData, unsigned int len)
{
	while (len > 0)
	{
		switch (m_Stage)
		{
		case ParseStage::Header:
		{
			unsigned int headerBytesNeed = g_MsgHeaderSize - m_HeaderBuffer.GetDataLength();
			unsigned int headerBytesToProcess = MIN(len, headerBytesNeed);
			m_HeaderBuffer.AppendFrom(pData, headerBytesToProcess);
			pData += headerBytesToProcess;
			len -= headerBytesToProcess;
			if (g_MsgHeaderSize == m_HeaderBuffer.GetDataLength())
			{
				if (m_pHeader->m_PayloadLength == 0)
				{
					OnMessageParsed();
				}
				else
				{
					m_Stage = ParseStage::Body;
					m_BodyBuffer.SetBufferSizeLowerBound(m_pHeader->m_PayloadLength);
					m_BodyBuffer.Clear();
					m_pMessage->m_ObjectBody = m_BodyBuffer.GetPtrForWriting();
				}
			}
		}
		break;
		case ParseStage::Body:
		{
			unsigned int bodyBytesNeed = m_pHeader->m_PayloadLength - m_BodyBuffer.GetDataLength();
			unsigned int bodyBytesToProcess = MIN(len, bodyBytesNeed);
			m_BodyBuffer.AppendFrom(pData, bodyBytesToProcess);
			pData += bodyBytesToProcess;
			len -= bodyBytesToProcess;
			if (m_pHeader->m_PayloadLength == m_BodyBuffer.GetDataLength())
			{
				OnMessageParsed();
			}
		}
		}
	}
}
void RdpStreamingParser::OnMessageParsed()
{
	m_Stage = ParseStage::Header;
	m_HeaderBuffer.Clear();
	m_BodyBuffer.Clear();
	m_MessageParsed(*m_pMessage);
}
SignalConnection RdpStreamingParser::RegisterMessageParsedEvent(OnMessageParsedAction action)
{
	return std::move(m_MessageParsed.connect(action));
}
