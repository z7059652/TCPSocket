#pragma once

#ifdef _WINRT_DLL

typedef enum tagPOINTER_BUTTON_CHANGE_TYPE {
	POINTER_CHANGE_NONE,
	POINTER_CHANGE_FIRSTBUTTON_DOWN,
	POINTER_CHANGE_FIRSTBUTTON_UP,
	POINTER_CHANGE_SECONDBUTTON_DOWN,
	POINTER_CHANGE_SECONDBUTTON_UP,
	POINTER_CHANGE_THIRDBUTTON_DOWN,
	POINTER_CHANGE_THIRDBUTTON_UP,
	POINTER_CHANGE_FOURTHBUTTON_DOWN,
	POINTER_CHANGE_FOURTHBUTTON_UP,
	POINTER_CHANGE_FIFTHBUTTON_DOWN,
	POINTER_CHANGE_FIFTHBUTTON_UP,
} POINTER_BUTTON_CHANGE_TYPE;

typedef struct tagPOINTER_INFO {
	DWORD           pointerType;
	UINT32          pointerId;
	UINT32          frameId;
	UINT32		    pointerFlags;
	HANDLE          sourceDevice;
	HWND            hwndTarget;
	POINT           ptPixelLocation;
	POINT           ptHimetricLocation;
	POINT           ptPixelLocationRaw;
	POINT           ptHimetricLocationRaw;
	DWORD           dwTime;
	UINT32          historyCount;
	INT32           InputData;
	DWORD           dwKeyStates;
	UINT64          PerformanceCount;
	POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;
} POINTER_INFO;

typedef struct tagPOINTER_TOUCH_INFO {
	POINTER_INFO    pointerInfo;
	UINT32			touchFlags;
	UINT32			touchMask;
	RECT            rcContact;
	RECT            rcContactRaw;
	UINT32          orientation;
	UINT32          pressure;
} POINTER_TOUCH_INFO;

typedef struct tagPOINTER_PEN_INFO {
	POINTER_INFO    pointerInfo;
	UINT32          penFlags;
	UINT32          penMask;
	UINT32          pressure;
	UINT32          rotation;
	INT32           tiltX;
	INT32           tiltY;
} POINTER_PEN_INFO;

enum tagPOINTER_INPUT_TYPE {
	PT_POINTER = 0x00000001,   // Generic pointer
	PT_TOUCH = 0x00000002,   // Touch
	PT_PEN = 0x00000003,   // Pen
	PT_MOUSE = 0x00000004,   // Mouse
#if(WINVER >= 0x0603)
	PT_TOUCHPAD = 0x00000005,   // Touchpad
#endif /* WINVER >= 0x0603 */
};
typedef DWORD POINTER_INPUT_TYPE;

#endif
