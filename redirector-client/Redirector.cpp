#include "Redirector.h"
#include "windows.h"
#include "iostream"
#include <freerdp/input.h>
#include "freerdp\client\rdpei.h"
#include "HIDSocketReader.h"
#include "TouchAndPenMsgReader.h"

using namespace Titanium;
using namespace Titanium::TIRA;
#define X_POS(lParam) ((UINT16) (lParam & 0xFFFF))
#define Y_POS(lParam) ((UINT16) ((lParam >> 16) & 0xFFFF))
#define IFCALL(_cb, ...) do { if (_cb != NULL) { _cb( __VA_ARGS__ ); } } while (0)
// buf: bad global object point

class RdpAgent *gRdpAgent;
extern "C" _declspec(dllimport) void(*gSendScreenData)(void *data, unsigned int size);
extern "C" _declspec(dllimport) void(*gReportState)(int state);
extern "C" _declspec(dllimport) void(*gRdpDisconnect)();
extern "C" _declspec(dllimport) void(*gSendHIDData)(void *data, unsigned int size);
extern "C" _declspec(dllimport) void(*gSendDRData)(void *data, unsigned int size);
extern "C" _declspec(dllimport) void(*ginjectAudioFormatInfo)(void *data, unsigned int size);
extern "C"  void(*gSendAudioplayData)(void *data, unsigned int size);
extern "C" _declspec(dllimport) rdpInput* gInPut;
extern "C" RdpeiClientContext* gMultitouchContext;
extern "C" UINT rdpei_send_touch_frame(RdpeiClientContext* context, POINTER_INFO *pointers, int count);
extern "C" UINT rdpei_send_pen_frame(RdpeiClientContext* context, POINTER_PEN_INFO *pen);

static const int DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE = 6000;
static const int DEFAULT_SOCKET_SEND_BUFFER_SIZE = 1400;
enum HIDType
{
	HID_MOUSE = 0,
	HID_KEYBOARD,
	HID_FOCUS_IN
};


void OnRdpDisconnect()
{
	if (gRdpAgent)
	{
		if (gRdpAgent->DisconnectEvent)
			gRdpAgent->DisconnectEvent();
	}
}
void OnScreendataReceive(void* buf, unsigned int len)
{
	if (gRdpAgent)
		gRdpAgent->ScreenDataReceivedEvent(buf, len);
	OutputDebugStringA((char*)buf);
}
void OnDRdataReceive(void* buf, unsigned int len)
{
	if (gRdpAgent)
		gRdpAgent->DRDataReceivedEvent(buf, len);
	OutputDebugStringA((char*)buf);
}
void OnAudiodataReceive(void* buf, unsigned int len)
{
	if (gRdpAgent)
		gRdpAgent->AudioDataReceivedEvent(buf, len);
	OutputDebugStringA((char*)buf);
}
void OnStateChanged(int status)
{
	static int statusOld = -1;
	if (status == statusOld)
	{
		return;
	}
	try
	{
		if (gRdpAgent)
			gRdpAgent->StageChangedEvent((RdpAgent::Stage)status);
	}
	catch (...)
	{
	}
	statusOld = status;
}
bool SendKeyBoardEvent(bool down, UINT32 rdp_scancode)
{
	UINT16 flags = (RDP_SCANCODE_EXTENDED(rdp_scancode) ? KBD_FLAGS_EXTENDED : 0) |
		((down) ? KBD_FLAGS_DOWN : KBD_FLAGS_RELEASE);
	UINT16 code = RDP_SCANCODE_CODE(rdp_scancode);
	gInPut->KeyboardEvent(gInPut, flags, code);
	return true;
}
Redirector::Redirector()
{
	_rdp = std::make_shared<RdpAgent>();
}

Redirector::~Redirector()
{
}
void Redirector::Start(const ProjectorSetting &projectorSetting, RdpSetting rdpSetting)
{
	try
	{
		_projector = std::make_shared<ProjectorAgent>(projectorSetting);
	}
	catch (...)
	{
		if (DisconnectedEvent)
			DisconnectedEvent(true);
		return;
	}
	_projector->Connect();
	Resolution resolution = _projector->ResquestResolution();
	gRdpAgent = _rdp.get();
	gSendScreenData = OnScreendataReceive;
	gSendDRData = OnDRdataReceive;
	gSendAudioplayData = OnAudiodataReceive;
	gReportState = OnStateChanged;
	gRdpDisconnect = OnRdpDisconnect;
	_rdp->StageChangedEvent = [this](RdpAgent::Stage stage)
	{
		if (stage == RdpAgent::Stage::CONNECTION_STATE_ACTIVE)
		{
			_projectorScreenSocket = _projector->SetupScreenChannel();
			_projectorHIDSocket = _projector->SetupHIDChannel();
			_HidDataReader = std::make_shared<HIDSocketReader>(_projectorHIDSocket);
			_HidDataReader->OnDataReceivedEvent = ([&](const HIDDATASTRUCT* pHidData)
			{
				if (pHidData->type == HIDType::HID_MOUSE)
				{
					if (gInPut&&gInPut->MouseEvent)
						gInPut->MouseEvent(gInPut, pHidData->Msg, UINT(pHidData->xPos), unsigned short(pHidData->yPos));
				}
				if (pHidData->type == HIDType::HID_KEYBOARD)
				{
					if (gInPut&&gInPut->KeyboardEvent)
					{
						gInPut->KeyboardEvent(gInPut, pHidData->Msg, pHidData->xPos);
					}
				}
				if (pHidData->type == HIDType::HID_FOCUS_IN)
				{
					if (gInPut&&gInPut->FocusInEvent)
						gInPut->FocusInEvent(gInPut, pHidData->Msg);
				}
			});
			_HidDataReader->Start();
			_drSocket = _projector->SetupDrChannel();
			_audioplaybackSocket = _projector->SetupAudioPlaybackChannel();
			_AudioDataReader = std::make_shared<AudioSocketReader>(_audioplaybackSocket);
			_AudioDataReader->OnDataReceivedEvent = ([&](void *data, unsigned int size)
			{
				if (ginjectAudioFormatInfo)
					ginjectAudioFormatInfo(data, size);
			});
			_AudioDataReader->Start();
			_toucAndPenSocket = _projector->SetupTouchAndPenChannel();
			_touchAndPenReader = std::make_shared<TouchAndPenMsgReader>(_toucAndPenSocket);
			_touchAndPenReader->OnTouchFrameReceivedEvent = [&](POINTER_INFO* pointers, int count)
			{
				if (gMultitouchContext)
					rdpei_send_touch_frame(gMultitouchContext, pointers, count);
			};
			_touchAndPenReader->OnPenFrameReceivedEvent = [&](POINTER_PEN_INFO* pen)
			{
				if (gMultitouchContext)
					rdpei_send_pen_frame(gMultitouchContext, pen);
			};
			_touchAndPenReader->Start();

		}
		if (OnStageChanged)
			OnStageChanged(stage);
	};
	_rdp->ScreenDataReceivedEvent = [this](void *data, unsigned int size)
	{
		if (_projectorScreenSocket)
		{
			_projectorScreenSocket->Send((const unsigned char*)&size, 4);
			_projectorScreenSocket->Send((const unsigned char*)data, size);
		}
	};
	_rdp->DRDataReceivedEvent = [this](void *data, unsigned int size)
	{
		if (_drSocket)
		{
			_drSocket->Send((const unsigned char*)&size, 4);
			_drSocket->Send((const unsigned char*)data, size);
		}
	};
	_rdp->AudioDataReceivedEvent = [this](void *data, unsigned int size)
	{
		if (_audioplaybackSocket)
		{
			_audioplaybackSocket->Send((const unsigned char*)&size, 4);
			_audioplaybackSocket->Send((const unsigned char*)data, size);
		}
	};
	_rdp->DisconnectEvent = [&]()
	{
		_projector->DisconnectedEvent = nullptr;
		_rdp->DisconnectEvent = nullptr;
		_projector.reset();
		if (DisconnectedEvent)
			DisconnectedEvent(false);
	};
	rdpSetting._resolution = resolution;
	_rdp->Connect(rdpSetting);
	_projector->DisconnectedEvent = [&]()
	{
		_projector->DisconnectedEvent = nullptr;
		_rdp->DisconnectEvent = nullptr;
		_rdp->Disconnect();
		if (DisconnectedEvent)
			DisconnectedEvent(true);
	};
}

void Redirector::Disconnect()
{
	if (_audioplaybackSocket)
	{
		_audioplaybackSocket->CancelRecv();
		_audioplaybackSocket->Shutdown();
	}
	_rdp->Disconnect();
}
