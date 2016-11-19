#pragma once
#include <memory>
#include "SocketTcp.h"
class Titanium::TIRA::SocketTcpServer;
class RDPProjectorPipeline;
class D2DRender;
class RdpScreenSource;
class HIDRedirector;
class AudioSource;
class AudioPlayback;
class RdpDRSource;
class DeviceRedirecr;
class TouchAndPen;
class RedirectChannelFactory
{
public:
	RedirectChannelFactory(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	~RedirectChannelFactory();
	void InitialRDPProjectorPipeline();
	void SetupScreenPipeline();
	void SetupHIDPipeline();
	void SetupDrRedirect();
	void SetupAudioPlaybackRedirect();
	void SetupTouchAndPenRedirect();
	void DestoryAllChannel();
private:
	std::shared_ptr<Titanium::TIRA::SocketTcpServer> m_Server;
	std::shared_ptr<RDPProjectorPipeline> m_RDPProjectorPipeline;
	std::shared_ptr<D2DRender> m_D2DRender;
	std::shared_ptr<RdpScreenSource> m_RdpSource;
	std::shared_ptr<HIDRedirector> m_HIDRedirector;
	std::shared_ptr<AudioSource> m_AudioSource;
	std::shared_ptr<AudioPlayback> m_AudioPlayback;
	std::shared_ptr<RdpDRSource> m_RdpDRSource;
	std::shared_ptr<DeviceRedirecr> m_DeviceRedirecr;
	std::shared_ptr<TouchAndPen> m_TouchAndPen;
};