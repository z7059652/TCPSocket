#include "RedirectChannelFactory.h"
#include "RDPProjectorPipeline.h"
#include "D2DRender.h"
#include "RdpScreenSource.h"
#include "HIDRedirector.h"
#include "SocketTcpServer.h"
#include "TcpChannel.h"
#include "AudioSource.h"
#include "AudioPlayback.h"
#include "RdpDRSource.h"
#include "DeviceRedirecr.h"
#include "TouchAndPen.h"
#include <functional>

static const int RDP_STREAMING_LISTENING_PORT = 5263;
static const int DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE = 6000;
static const int DEFAULT_SOCKET_SEND_BUFFER_SIZE = 1400;
using namespace std;
using namespace Titanium;
using namespace Titanium::TIRA;
extern "C"
{
#include "rdpdr_main.h"
#include "rdpsnd_main.h"
}
RedirectChannelFactory::RedirectChannelFactory(std::shared_ptr<Titanium::TIRA::SocketTcp> socket) :m_Server(std::make_shared<SocketTcpServer>(RDP_STREAMING_LISTENING_PORT))
{
	m_Server->StartListening();
}
RedirectChannelFactory::~RedirectChannelFactory()
{
	m_Server->StopListening();
}
void RedirectChannelFactory::InitialRDPProjectorPipeline()
{
	m_RDPProjectorPipeline = std::make_shared<RDPProjectorPipeline>();
	m_RDPProjectorPipeline->Start();
	drdynvcPlugin* drdynvc = (drdynvcPlugin*)m_RDPProjectorPipeline->QuerydrdynvcPlugin();
	rdpsndPlugin* rdpsnd = (rdpsndPlugin*)m_RDPProjectorPipeline->QueryrdpsndPlugin();
	rdpdrPlugin* rdpdr = (rdpdrPlugin*)m_RDPProjectorPipeline->QueryrdpdrPlugin();
	if (drdynvc == nullptr || rdpsnd == nullptr || rdpdr == nullptr)
		throw runtime_error("Plugin init error.");
	m_D2DRender = std::make_shared<D2DRender>(drdynvc);
	m_AudioPlayback = std::make_shared<AudioPlayback>(rdpsnd);
	m_DeviceRedirecr = std::make_shared<DeviceRedirecr>(rdpdr);

}
void RedirectChannelFactory::SetupScreenPipeline()
{
	auto socket = m_Server->WaitForConnection();
	socket->SetTcpNoDelay(true);
	socket->SetRecvBufferSize(DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE);
	socket->SetSendBufferSize(DEFAULT_SOCKET_SEND_BUFFER_SIZE);
	auto videosocket = std::make_shared<TcpChannel>(socket);
	m_RdpSource = std::make_shared<RdpScreenSource>(videosocket);
	(*m_RdpSource) >> (*m_D2DRender);
	m_RdpSource->Start();
	m_D2DRender->Start();
}
void RedirectChannelFactory::SetupHIDPipeline()
{
	auto socket = m_Server->WaitForConnection();
	socket->SetTcpNoDelay(true);
	socket->SetRecvBufferSize(DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE);
	socket->SetSendBufferSize(DEFAULT_SOCKET_SEND_BUFFER_SIZE);
	auto hidsocket = std::make_shared<TcpChannel>(socket);
	m_HIDRedirector = std::make_shared<HIDRedirector>(hidsocket);
	m_HIDRedirector->Start();
}
void RedirectChannelFactory::SetupDrRedirect()
{
	auto socket = m_Server->WaitForConnection();
	socket->SetTcpNoDelay(true);
	socket->SetRecvBufferSize(DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE);
	socket->SetSendBufferSize(DEFAULT_SOCKET_SEND_BUFFER_SIZE);
	auto drsocket = std::make_shared<TcpChannel>(socket);
	m_RdpDRSource = std::make_shared<RdpDRSource>(drsocket);
	(*m_RdpDRSource) >> (*m_DeviceRedirecr);
	m_RdpDRSource->Start();
	m_DeviceRedirecr->Start();
}
void RedirectChannelFactory::SetupAudioPlaybackRedirect()
{
	auto socket = m_Server->WaitForConnection();
	socket->SetTcpNoDelay(true);
	socket->SetRecvBufferSize(DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE);
	socket->SetSendBufferSize(DEFAULT_SOCKET_SEND_BUFFER_SIZE);
	auto audiosocket = std::make_shared<TcpChannel>(socket);
	m_AudioSource = std::make_shared<AudioSource>(audiosocket);
	(*m_AudioSource) >> (*m_AudioPlayback);
	m_AudioSource->Start();
	m_AudioPlayback->Start();
}
void RedirectChannelFactory::SetupTouchAndPenRedirect()
{
	auto socket = m_Server->WaitForConnection();
	socket->SetTcpNoDelay(true);
	socket->SetRecvBufferSize(DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE);
	socket->SetSendBufferSize(DEFAULT_SOCKET_SEND_BUFFER_SIZE);
	auto touchAndPenChannel = std::make_shared<TcpChannel>(socket);
	m_TouchAndPen = std::make_shared<TouchAndPen>(touchAndPenChannel);
}
void RedirectChannelFactory::DestoryAllChannel()
{
	// stop oreder must like this ,because of the dependence of each module
	if (m_HIDRedirector)
		m_HIDRedirector->Stop();
	if (m_RdpSource)
		m_RdpSource->Stop();
	if (m_RDPProjectorPipeline)
		m_RDPProjectorPipeline->Stop();
	if (m_D2DRender)
		m_D2DRender->Stop();
	if (m_RdpDRSource)
		m_RdpDRSource->Stop();
	if (m_DeviceRedirecr)
		m_DeviceRedirecr->Stop();
	if (m_AudioSource)
		m_AudioSource->Stop();
	if (m_AudioPlayback)
		m_AudioPlayback->Stop();
	if (m_TouchAndPen)
		m_TouchAndPen.reset();
}