#pragma once
#include <string>
#include <memory>
#include "ProjectorAgent.h"
#include "ProjectorSetting.h"
#include "RdpAgent.h"
#include "RdpSetting.h"
#include "Socket.h"
#include "HIDSocketReader.h"
#include "TouchAndPenMsgReader.h"
#include "AudioSocketReader.h"
class Redirector
{
public:
	Redirector();
	virtual ~Redirector();
	void Start(const ProjectorSetting &projectorSetting, RdpSetting rdpSetting);
	void Disconnect();

	std::function<void(int)> OnStageChanged;
	std::function<void(bool)> DisconnectedEvent;
private:
	std::shared_ptr<ProjectorAgent> _projector;
	std::shared_ptr<RdpAgent> _rdp;
	std::shared_ptr<Titanium::TIRA::Socket> _projectorScreenSocket;
	std::shared_ptr<Titanium::TIRA::SocketTcp> _projectorHIDSocket;
	std::shared_ptr<Titanium::TIRA::SocketTcp> _drSocket;
	std::shared_ptr<Titanium::TIRA::SocketTcp> _audioplaybackSocket;
	std::shared_ptr<Titanium::TIRA::SocketTcp> _toucAndPenSocket;
	std::shared_ptr<HIDSocketReader> _HidDataReader;
	std::shared_ptr<AudioSocketReader> _AudioDataReader;
	std::shared_ptr<TouchAndPenMsgReader> _touchAndPenReader;
};