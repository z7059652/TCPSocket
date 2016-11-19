#pragma once
#include <functional>
#include <memory>
#include "SignalConnection.h"
#include "networkdefs.h"

namespace Titanium{
	/// Defines methods of the channel.
	struct ChannelState
	{
		ChannelState()
		{
			availbelbitrate = 0;
			losspacketsrate = 0;	
			bytesReceived = 0;
		}

		ChannelState(const ChannelState& ioMetrics)
		{
			availbelbitrate = ioMetrics.availbelbitrate;
			losspacketsrate = ioMetrics.losspacketsrate;
			bytesReceived = ioMetrics.bytesReceived;
		}

		ChannelState& operator=(const ChannelState& ioMetrics)
		{
			availbelbitrate = ioMetrics.availbelbitrate;
			losspacketsrate = ioMetrics.losspacketsrate;
			bytesReceived = ioMetrics.bytesReceived;

			return *this;
		}

		uint32_t availbelbitrate;           //!< The current bit rate calculated by channel
		double  losspacketsrate;   //!< The rate of lost packets
		uint64_t bytesReceived;
	};
	class IChannel
	{
	public:
		typedef std::function<void(const unsigned char* pData, unsigned int dataLen)> OnDataReceivedAction;
	public:
		/// Starts the worker.
		virtual void Start() = 0;
		/// Stops the worker.
		virtual void Stop() = 0;
		/// Puts data to internal buffer, Flush method will be called automatically if the internal buffer is full.
		virtual void Send(const unsigned char* pData, unsigned int dataLen) = 0;
		/// Cancel pending sending request.
		virtual void CancelSend() = 0;
		/// Sends all buffered data.
		virtual void Flush() = 0;
		/// Occurs when receives data.
		virtual TIRA::SignalConnection RegisterDataReceivedEvent(OnDataReceivedAction action) = 0;
		/// inquiry channel ChannelState.
		virtual bool GetChannelState(ChannelState& curChannelState) = 0;
		/// Get channel Name.
		virtual std::string GetChannelName() = 0;
		/// Set channel Name.
		virtual void SetChannelName(const std::string &SetName) = 0;

	protected:
		unsigned char m_ChannelID;
		std::string m_ChannelName;
	};
}
