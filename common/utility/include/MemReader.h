#pragma once

namespace Titanium { namespace TIRA
{
	class MemReader
	{
	public:
		MemReader(const unsigned char *buf, unsigned int size);

		unsigned char ReadUINT8();
		char ReadINT8();
		unsigned short ReadUINT16();
		short ReadINT16();
		unsigned int ReadUINT32();
		int ReadINT32();
		unsigned long long ReadUINT64();
		long long ReadINT64();
	private:
		const unsigned char *m_Buf;
		unsigned int m_Size;
		unsigned int m_BytesConsumed;
	};
}
}
