#pragma once
#include <functional>
#include "SignalConnection.h"
#include "DataBuffer.h"

struct RdpStreamingMessage;
struct RdpStreamingMsgHeader;
using namespace Titanium;
using namespace Titanium::TIRA;
class RdpStreamingParser
{
public:
	typedef std::function<void(const RdpStreamingMessage &msg)> OnMessageParsedAction;
	RdpStreamingParser();
	void Process(const unsigned char* pData, unsigned int len);
	SignalConnection RegisterMessageParsedEvent(OnMessageParsedAction);
protected:
	void OnMessageParsed();
private:
	enum class ParseStage
	{
		Header,
		Body
	};
	Signal<void(const RdpStreamingMessage &msg)> m_MessageParsed;
	DataBuffer<unsigned char> m_BodyBuffer;
	DataBuffer<unsigned char> m_HeaderBuffer;
	RdpStreamingMsgHeader *m_pHeader;
	RdpStreamingMessage *m_pMessage;
	ParseStage m_Stage;
};
