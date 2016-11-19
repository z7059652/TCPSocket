#include "ReadableSource.h"
using namespace Titanium::TIRA;

TI_Resolution ReadableSource::ReadResolution()
{
	return Read<TI_Resolution>();
}

unsigned long long ReadableSource::ReadUInt64()
{
	return Read<unsigned long long>();
}

bool ReadableSource::ReadBoolean()
{
	return Read<unsigned char>() > 0;
}

unsigned char ReadableSource::ReadByte()
{
	return Read<unsigned char>();
}

float ReadableSource::ReadSingle()
{
	return Read<float>();
}

double ReadableSource::ReadDouble()
{
	return Read<double>();
}

short ReadableSource::ReadInt16()
{
	return Read<short>();
}

int ReadableSource::ReadInt32()
{
	return Read<int>();
}

long long ReadableSource::ReadInt64()
{
	return Read<long long>();
}

char ReadableSource::ReadChar()
{
	return Read<char>();
}

wchar_t ReadableSource::ReadWchar()
{
	return Read<wchar_t>();
}

unsigned short ReadableSource::ReadUInt16()
{
	return Read<unsigned short>();
}

unsigned int ReadableSource::ReadUInt32()
{
	return Read<unsigned int>();
}