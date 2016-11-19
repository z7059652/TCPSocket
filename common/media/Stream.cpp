#include "Stream.h"

Stream::Stream(size_t size)
	: buffer(new unsigned char[size], [](unsigned char *p) {delete[]p; }), m_bufferSize(size)
{
}

Stream::~Stream()
{
}