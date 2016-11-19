#include "freerdp\client\rdpei.h"
#include "windows.h"
UINT rdpei_send_touch_frame(RdpeiClientContext* context, POINTER_INFO *pointers, int count);
UINT32 lastFrameId = -1;

extern RdpeiClientContext* gMultitouchContext;

void gHandleMultitouch(POINTER_INFO *pointers, int count)
{
	if (lastFrameId == pointers[0].frameId)
		return;
	lastFrameId = pointers[0].frameId;
	if (gMultitouchContext == NULL)
		return;
	if (pointers[0].pointerType == PT_TOUCH)
		rdpei_send_touch_frame(gMultitouchContext, pointers, count);
	else if (pointers[0].pointerType == PT_PEN)
	{
		POINTER_PEN_INFO penInfo;
		GetPointerPenInfo(pointers[0].pointerId, &penInfo);
		rdpei_send_pen_frame(gMultitouchContext, &penInfo);
	}
}