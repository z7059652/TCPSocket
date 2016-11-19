#include "TouchAndPenMsgReader.h"
#include <vector>
using namespace std;

struct POINTER_DATA
{
	int count;
	POINTER_INFO pointers[10];
};
TouchAndPenMsgReader::TouchAndPenMsgReader(std::shared_ptr<Titanium::TIRA::SocketTcp> socket)
	:AsyncBase("TouchAndPenMsgReader")
	, m_socket(socket)
{
}

TouchAndPenMsgReader::~TouchAndPenMsgReader()
{
}

void TouchAndPenMsgReader::Start()
{
	StartAsyncBase();
}

void TouchAndPenMsgReader::Stop()
{
	StopAsyncBase();
}

void TouchAndPenMsgReader::Workloop()
{
	POINTER_DATA pointerData;
	unsigned char *buf = (unsigned char *)&pointerData;
	unsigned char* bufpointer = buf;
	int len = 0;
	while (CanLoopContinue())
	{
		int lentemp = m_socket->Recv(bufpointer, min(sizeof(POINTER_DATA), sizeof(POINTER_DATA) - len));
		len += lentemp;
		bufpointer += lentemp;
		if (len == sizeof(POINTER_DATA))
		{
			if (pointerData.pointers[0].pointerType == PT_TOUCH)
			{
				if (OnTouchFrameReceivedEvent)
					OnTouchFrameReceivedEvent(pointerData.pointers, pointerData.count);
			}
			else if (pointerData.pointers[0].pointerType == PT_PEN)
			{
				if (OnPenFrameReceivedEvent)
					OnPenFrameReceivedEvent((POINTER_PEN_INFO*)pointerData.pointers);
			}
			len = 0;
			memset(buf, 0, sizeof(POINTER_DATA));
			bufpointer = buf;
		}
	}
}

void TouchAndPenMsgReader::Cancel()
{
	m_socket->CancelRecv();
}
