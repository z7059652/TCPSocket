#include "MemWriter.h"
#include "error_handling_utility.h"
using namespace Titanium::TIRA;

MemWriter::MemWriter(unsigned char *buf, unsigned int size)
	: m_Buf(buf)
	, m_Size(size)
	, m_BytesConsumed(0)
{
}
void MemWriter::WriteUINT8(unsigned char x)
{
	ENSURE(m_BytesConsumed + sizeof(x) <= m_Size);
	*m_Buf = x;
	m_Buf++;
	m_BytesConsumed++;
}
void MemWriter::WriteINT8(char x)
{
	WriteUINT8(x);
}
void MemWriter::WriteUINT16(unsigned short x)
{
	WriteINT8((unsigned char)(x >> 8));
	WriteINT8((unsigned char)(x & 0xff));
}
void MemWriter::WriteINT16(short x)
{
	WriteUINT16(x);
}
void MemWriter::WriteUINT32(unsigned int x)
{
	WriteUINT16((unsigned short)(x >> 16));
	WriteUINT16((unsigned short)(x & 0xffff));
}
void MemWriter::WriteINT32(int x)
{
	WriteUINT32(x);
}
void MemWriter::WriteUINT64(unsigned long long x)
{
	WriteUINT32((unsigned int)(x >> 32));
	WriteUINT32((unsigned int)(x & 0xffffffff));
}
void MemWriter::WriteINT64(long long x)
{
	WriteUINT64(x);
}