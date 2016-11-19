#include "Session.h"
#include "SocketTcp.h"
#include "RedirectChannelFactory.h"
#include "TcpChannel.h"
#include "RtspSession.h"
#include <iostream>
#include <sstream>
#include <memory>

using namespace std;
using namespace Titanium;
using namespace Titanium::TIRA;

Session::Session(shared_ptr<SocketTcp> socket)
	: _rtsp(make_shared<RtspSession>(socket))
	, m_RedirectChannelFactory(std::make_shared<RedirectChannelFactory>(socket))
{
}

Session::~Session()
{
	Stop();
	m_RedirectChannelFactory.reset();
}

void Session::Start()
{
	_rtsp->ReceivedRequestEvent = [this](const RequestMsg &request)
	{
		if (request.GetCmd() == "RESOLUTION")
		{
			ostringstream os;
			int w = GetSystemMetrics(SM_CXSCREEN);
			int h = GetSystemMetrics(SM_CYSCREEN);
			os << w << 'x' << h;
			ResponseMsg response(request.GetCSeq(), true, os.str());
			_rtsp->Send(response);
		}
		else if (request.GetCmd() == "SETUP_SCREEN_CHANNEL")
		{
			try
			{
				m_RedirectChannelFactory->InitialRDPProjectorPipeline();
				_rtsp->Send(ResponseMsg(request.GetCSeq(), true));
				m_RedirectChannelFactory->SetupScreenPipeline();
			}
			catch (std::exception &e)
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), false, e.what()));
			}
		}
		else if (request.GetCmd() == "SETUP_HID_CHANNEL")
		{
			try
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), true));
				m_RedirectChannelFactory->SetupHIDPipeline();
			}
			catch (std::exception &e)
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), false, e.what()));
			}
		}
		else if (request.GetCmd() == "SETUP_DR_CHANNEL")
		{
			try
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), true));
				m_RedirectChannelFactory->SetupDrRedirect();
			}
			catch (std::exception &e)
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), false, e.what()));
			}
		}
		else if (request.GetCmd() == "SETUP_AUDIOPLAYBACK_CHANNEL")
		{
			try
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), true));
				m_RedirectChannelFactory->SetupAudioPlaybackRedirect();
			}
			catch (std::exception &e)
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), false, e.what()));
			}
		}
		else if (request.GetCmd() == "SETUP_TOUCHANDPEN_CHANNEL")
		{
			try
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), true));
				m_RedirectChannelFactory->SetupTouchAndPenRedirect();
			}
			catch (std::exception &e)
			{
				_rtsp->Send(ResponseMsg(request.GetCSeq(), false, e.what()));
			}
		}
	};
	_rtsp->DisconnectedByClientEvent = [this]()
	{
		DisconnectedEvent(this);
	};
	_rtsp->Start();
}

void Session::Stop()
{
	_rtsp->Stop();
	m_RedirectChannelFactory->DestoryAllChannel();
}
