#pragma once
#include "target_os.h"
#include <string>
#ifdef __GNUC__
#define WM_USER                         0x0400
#endif

namespace Titanium { namespace TIRA
{
	const unsigned int maxSupportPointerCount = 10;


	//Message Type
	const unsigned short uHidType_H2D_Cont = 0x03 << 8 | 0x00;
	const unsigned short uHidType_H2D_Cont_MOUSE = 0x0060;
	const unsigned short uHidType_H2D_Cont_KEY = 0x0070;
	const unsigned short uHidype_H2D_Cont_TOUCH = 0x0080;
	const unsigned short uHidType_H2D_Cont_PEN = 0x0090;


	const char chPointerDownMask = 0x01;
	const char chPointerUpMask = 0x02;
	const char chPointerUpdateMask = 0x04;

	const char chLButtonDownMask = 0x01;
	const char chMButtonDownMask = 0x02;
	const char chRButtonDownMask = 0x04;
	const char chWheelRotateForward = 0x08;
	const char chWheelRotateBackward = 0x10;

	const char chPenFirstButton = 0x1;
	const char chPenSecondButton = 0x2;
	const char chPenInRange = 0x8;
	const char chPenInContact = 0x10;

	enum Ti_HID_Type
	{
		MsgTypeMouse = 0x60,
		MsgTypeKey = 0x70,
		MsgTypeTouch = 0x80,
		MsgTypePen = 0x90
	};

	class HIDMessage
	{

	};

	class MouseMessage : public HIDMessage
	{
	public:
		char chMask;
		short wPositionX;
		short wPositionY;
	};

	class KeyMessage : public HIDMessage
	{
	public:
		char chMask;
		short wKeyValue;
	};

	class TouchMessage : public HIDMessage
	{
	public:
		char chMask;
		short wPositionX;
		short wPositionY;
		UINT32 id;
	};

	class PenMessage : public HIDMessage
	{
	public:
		char chMask;
		short wPositionX;
		short wPositionY;
		unsigned int penFlags;
		unsigned int penMask;
		unsigned int pressure;
		unsigned int rotation;
		int	tiltX;
		int tiltY;
	};

	const unsigned int MouseMessageSizeInBytes = sizeof(MouseMessage);
	const unsigned int KeyMessageSizeInBytes = sizeof(KeyMessage);
	const unsigned int TouchMessageSizeInBytes = sizeof(TouchMessage);
	const unsigned int PenMessageSizeInBytes = sizeof(PenMessage);
}
}
