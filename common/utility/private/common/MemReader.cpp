#include "MemReader.h"
#include "error_handling_utility.h"
using namespace Titanium::TIRA;

MemReader::MemReader(const unsigned char *buf, unsigned int size)
	: m_Buf(buf)
	, m_Size(size)
	, m_BytesConsumed(0)
{
}
unsigned char MemReader::ReadUINT8()
{
	ENSURE(m_BytesConsumed + sizeof(unsigned char) <= m_Size);
	unsigned char x = *m_Buf;
	m_Buf++;
	m_BytesConsumed++;
	return x;
}
char MemReader::ReadINT8()
{
	return ReadUINT8();
}
unsigned short MemReader::ReadUINT16()
{
	unsigned short x = ReadUINT8() << 8;
	x += ReadUINT8();
	return x;
}
short MemReader::ReadINT16()
{
	return ReadUINT16();
}
unsigned int MemReader::ReadUINT32()
{
	unsigned int x = ReadUINT16() << 16;
	x += ReadUINT16();
	return x;
}
int MemReader::ReadINT32()
{
	return ReadUINT32();
}
unsigned long long MemReader::ReadUINT64()
{
	unsigned long long x = (unsigned long long)ReadUINT32() << 32;
	x += ReadUINT32();
	return x;
}
long long MemReader::ReadINT64()
{
	return ReadUINT64();
}