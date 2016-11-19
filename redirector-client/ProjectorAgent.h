#pragma once
#include "RtspSession.h"
#include "IAgent.h"
#include "Socket.h"
#include "Resolution.h"
#include "ProjectorSetting.h"
#include <functional>
#include <mutex>
#include <condition_variable>

class ProjectorAgent :IAgent
{
public:
	ProjectorAgent(const ProjectorSetting &setting);
	~ProjectorAgent();

	void Connect();
	void Disconnect();

	std::function<void()> DisconnectedEvent;
	Resolution ResquestResolution();
	std::shared_ptr<Titanium::TIRA::SocketTcp> SetupScreenChannel();
	std::shared_ptr<Titanium::TIRA::SocketTcp> SetupHIDChannel();
	std::shared_ptr<Titanium::TIRA::SocketTcp> SetupDrChannel();
	std::shared_ptr<Titanium::TIRA::SocketTcp> SetupAudioPlaybackChannel();
	std::shared_ptr<Titanium::TIRA::SocketTcp> SetupTouchAndPenChannel();
private:
	Resolution ParseResolution(const std::string &str);
	
	ProjectorSetting _setting;
	RtspSession _rtsp;

	std::mutex _mutex;
	std::condition_variable _cv;

	Resolution _resolution;
	RequestMsg _resolutionRequest;
	RequestMsg _setupScreenChannelRequest;
	RequestMsg _setupHIDChannelRequest;
	RequestMsg _setupDrChannelRequest;
	RequestMsg _setupAudioPlaybackChannelRequest;
	RequestMsg _setupTouchAndPenChannelRequest;
	bool _resolutionReceived;
	bool _setupScreenChannelReceived;
	bool _setupScreenChannelResult;
	bool _canncelled;
	bool _setupHIDChannelReceived;
	bool _setupDrChannelReceived;
	bool _setupAudioPlaybackChannelReceived;
	bool _setupTouchAndPenChannelReceived;
};

