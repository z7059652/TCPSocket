#include "TouchAndPen.h"
#include <mutex>

using namespace Titanium;
using namespace std;

static TouchAndPen *gTouchAndPen = nullptr;
static mutex _mtx;

extern "C" void(*gHandleMultitouch)(POINTER_INFO *pointers, int count);
extern "C" void(MultitouchHandler)(POINTER_INFO *pointers, int count)
{
	lock_guard<mutex> lock(_mtx);
	if (gTouchAndPen == nullptr)
		return;
	gTouchAndPen->Send(pointers, count);
}

TouchAndPen::TouchAndPen(shared_ptr<TcpChannel> channel)
	: _channel(channel)
	, lastFrameId(-1)
{
	gHandleMultitouch = MultitouchHandler;
	_channel->Start();
	gTouchAndPen = this;
}

TouchAndPen::~TouchAndPen()
{
	_channel->CancelSend();
	_channel->Stop();
	lock_guard<mutex> lock(_mtx);
	gTouchAndPen = nullptr;
}

void TouchAndPen::Send(POINTER_INFO *pointers, int count)
{
	if (lastFrameId == pointers[0].frameId)
		return;
	lastFrameId = pointers[0].frameId;
	struct POINTER_DATA
	{
		int count;
		POINTER_INFO pointers[10];
	} buf;
	if (pointers[0].pointerType == PT_TOUCH)
	{
		buf.count = count;
		for (size_t i = 0; i < count; i++)
			buf.pointers[i] = pointers[i];
		_channel->Send((const unsigned char*)&buf, sizeof(buf));
	}
	else if (pointers[0].pointerType == PT_PEN)
	{
		buf.count = 1;
		GetPointerPenInfo(pointers[0].pointerId, (POINTER_PEN_INFO*)buf.pointers);
		_channel->Send((const unsigned char*)&buf, sizeof(buf));
	}
}
