#include "ProjectorAgent.h"
#include "SocketTcp.h"

using namespace std;
using namespace Titanium::TIRA;

ProjectorAgent::ProjectorAgent(const ProjectorSetting &setting)
: _setting(setting)
, _rtsp(SocketTcp::ConnectTo(setting.GetNegotiationAddress()))
, _resolutionRequest("RESOLUTION")
, _setupScreenChannelRequest("SETUP_SCREEN_CHANNEL")
, _setupHIDChannelRequest("SETUP_HID_CHANNEL")
, _setupDrChannelRequest("SETUP_DR_CHANNEL")
, _setupAudioPlaybackChannelRequest("SETUP_AUDIOPLAYBACK_CHANNEL")
, _setupTouchAndPenChannelRequest("SETUP_TOUCHANDPEN_CHANNEL")
, _resolutionReceived(false)
, _setupScreenChannelReceived(false)
, _setupScreenChannelResult(false)
, _canncelled(false)
, _setupHIDChannelReceived(false)
, _setupDrChannelReceived(false)
, _setupAudioPlaybackChannelReceived(false)
, _setupTouchAndPenChannelReceived(false)
{
	_rtsp.DisconnectedByClientEvent = [this]()
	{
		if (DisconnectedEvent != nullptr)
			DisconnectedEvent();
	};
	_rtsp.ReceivedResponseEvent = [this](const ResponseMsg &response)
	{
		if (_resolutionRequest.GetCSeq() == response.GetCSeq())
		{
			unique_lock<mutex> lock(_mutex);
			_resolution = ParseResolution(response.GetData());
			_resolutionReceived = true;
			_cv.notify_one();
		}
		else if (_setupScreenChannelRequest.GetCSeq() == response.GetCSeq())
		{
			unique_lock<mutex> lock(_mutex);
			_setupScreenChannelResult = response.GetResult();
			_setupScreenChannelReceived = true;
			_cv.notify_one();
		}
		else if (_setupHIDChannelRequest.GetCSeq() == response.GetCSeq())
		{
			unique_lock<mutex> lock(_mutex);
			_setupHIDChannelReceived = true;
			_cv.notify_one();
		}
		else if (_setupDrChannelRequest.GetCSeq() == response.GetCSeq())
		{
			unique_lock<mutex> lock(_mutex);
			_setupDrChannelReceived = true;
			_cv.notify_one();
		}
		else if (_setupAudioPlaybackChannelRequest.GetCSeq() == response.GetCSeq())
		{
			unique_lock<mutex> lock(_mutex);
			_setupAudioPlaybackChannelReceived = true;
			_cv.notify_one();
		}
		else if (_setupTouchAndPenChannelRequest.GetCSeq() == response.GetCSeq())
		{
			unique_lock<mutex> lock(_mutex);
			_setupTouchAndPenChannelReceived = true;
			_cv.notify_one();
		}
	};
}

void ProjectorAgent::Connect()
{
	_rtsp.Start();
}

void ProjectorAgent::Disconnect()
{
	_rtsp.Stop();
}

ProjectorAgent::~ProjectorAgent()
{
	unique_lock<mutex> lock(_mutex);
	_canncelled = true;
	_cv.notify_all();
	_rtsp.Stop();
}

Resolution ProjectorAgent::ResquestResolution()
{
	unique_lock<mutex> lock(_mutex);
	_rtsp.Send(_resolutionRequest);
	if (!_cv.wait_for(lock, chrono::seconds(51), [this]() {return _resolutionReceived || _canncelled; })) // todo: move 5 seconds somewhere else
		throw runtime_error("Wait for rtsp response timeout.");
	return _resolution;
}

std::shared_ptr<SocketTcp> ProjectorAgent::SetupScreenChannel()
{
	unique_lock<mutex> lock(_mutex);
	_rtsp.Send(_setupScreenChannelRequest);
	if (!_cv.wait_for(lock, chrono::seconds(51), [this]() {return _setupScreenChannelReceived || _canncelled; })) // todo: move 5 seconds somewhere else
		throw runtime_error("Wait for rtsp response timeout.");
	if (!_setupScreenChannelResult)
		throw runtime_error("Projector cannot setup screen channel.");
	return SocketTcp::ConnectTo(_setting.GetMeidaAddress());
}



std::shared_ptr<Titanium::TIRA::SocketTcp> ProjectorAgent::SetupHIDChannel()
{
	unique_lock<mutex> lock(_mutex);
	_rtsp.Send(_setupHIDChannelRequest);
	if (!_cv.wait_for(lock, chrono::seconds(51), [this]() {return _setupHIDChannelReceived || _canncelled; })) // todo: move 5 seconds somewhere else
		throw runtime_error("Wait for rtsp setupHIDChannel response timeout.");
	if (!_setupHIDChannelReceived)
		throw runtime_error("Projector cannot setup screen channel.");

	return SocketTcp::ConnectTo(_setting.GetMeidaAddress());
}
std::shared_ptr<Titanium::TIRA::SocketTcp> ProjectorAgent::SetupDrChannel()
{
	unique_lock<mutex> lock(_mutex);
	_rtsp.Send(_setupDrChannelRequest);
	if (!_cv.wait_for(lock, chrono::seconds(51), [this]() {return _setupDrChannelReceived || _canncelled; })) // todo: move 5 seconds somewhere else
		throw runtime_error("Wait for rtsp SetupDrChannel response timeout.");
	if (!_setupDrChannelReceived)
		throw runtime_error("Projector cannot setup screen channel.");

	return SocketTcp::ConnectTo(_setting.GetMeidaAddress());
}
std::shared_ptr<Titanium::TIRA::SocketTcp> ProjectorAgent::SetupAudioPlaybackChannel()
{
	unique_lock<mutex> lock(_mutex);
	_rtsp.Send(_setupAudioPlaybackChannelRequest);
	if (!_cv.wait_for(lock, chrono::seconds(51), [this]() {return _setupAudioPlaybackChannelReceived || _canncelled; })) // todo: move 5 seconds somewhere else
		throw runtime_error("Wait for rtsp SetupAudioPlaybackChannel response timeout.");
	if (!_setupAudioPlaybackChannelReceived)
		throw runtime_error("Projector cannot setup screen channel.");

	return SocketTcp::ConnectTo(_setting.GetMeidaAddress());
}
std::shared_ptr<Titanium::TIRA::SocketTcp> ProjectorAgent::SetupTouchAndPenChannel()
{
	unique_lock<mutex> lock(_mutex);
	_rtsp.Send(_setupTouchAndPenChannelRequest);
	if (!_cv.wait_for(lock, chrono::seconds(51), [this]() {return _setupTouchAndPenChannelReceived || _canncelled; })) // todo: move 5 seconds somewhere else
		throw runtime_error("Wait for rtsp SetupAudioPlaybackChannel response timeout.");
	if (!_setupTouchAndPenChannelReceived)
		throw runtime_error("Projector cannot setup screen channel.");

	return SocketTcp::ConnectTo(_setting.GetMeidaAddress());
}
Resolution ProjectorAgent::ParseResolution(const std::string & str)
{
	istringstream is(str);
	Resolution resolution;
	char tmpChar;
	is >> resolution.w >> tmpChar >> resolution.h;
	return resolution;
}
