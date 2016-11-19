#pragma once
#include <memory>
#include "AsyncBase.h"
#include "DataBuffer.h"
#include "noncopyable.h"
#include "SocketTcp.h"
#include "IChannel.h"

namespace Titanium
{
	namespace TIRA
	{
		class Socket;
		class WritableSource;
		class ReadableSource;
	}

	class TcpChannel : public TIRA::AsyncBase, public IChannel, public TIRA::NonCopyable
	{
	public:
		explicit TcpChannel(std::shared_ptr<TIRA::SocketTcp> socket);
		~TcpChannel();
	public:
		virtual void Start();
		virtual void Stop();
		virtual bool IsInError();
	public:
		virtual void Send(const unsigned char* pData, unsigned int dataLen);
		virtual void CancelSend();
		virtual void Flush();
		virtual TIRA::SignalConnection RegisterDataReceivedEvent(OnDataReceivedAction action);
		virtual std::string GetChannelName();
		virtual void SetChannelName(const std::string& SetName);
		virtual bool GetChannelState(ChannelState& curGetChannelState);
	protected:
		virtual void OnDataReceived(const unsigned char* pData, unsigned int len);


	protected:
		virtual void Workloop();
		virtual void UnBlockWorkingThread();
	private:
		TIRA::DataBuffer<unsigned char> m_InternalBuffer;
		TIRA::DataBuffer<unsigned char> m_InternalWritingBuffer;

		std::shared_ptr<TIRA::WritableSource> m_Writer;
		std::shared_ptr<TIRA::ReadableSource> m_Reader;

		TIRA::Signal<void(const unsigned char*, unsigned int)> m_SignalDataReceived;
	};
}