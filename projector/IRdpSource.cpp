#include "IRdpSource.h"
#include "RdpStreamingDef.h"
#include "RdpStreamingParser.h"


IRdpSource::IRdpSource(std::shared_ptr<IChannel> channel) :m_Channel(channel), m_Parser(std::make_shared<RdpStreamingParser>())
{

}
IRdpSource::~IRdpSource()
{
	Stop();
}
void IRdpSource::Start()
{
	m_Channel->RegisterDataReceivedEvent([&](const unsigned char *data, int size)
	{
		m_Parser->Process(data, size);
	});
	m_Parser->RegisterMessageParsedEvent([&](const RdpStreamingMessage &msg)
	{
		Stream stream(msg.m_ObjectHeader.m_PayloadLength);
		memcpy(stream.buffer.get(), msg.m_ObjectBody, msg.m_ObjectHeader.m_PayloadLength);
		_out->Push(stream);
	});
	m_Channel->Start();
}
void IRdpSource::Stop()
{
	m_Channel->Stop();
}
void IRdpSource::Process()
{
}
