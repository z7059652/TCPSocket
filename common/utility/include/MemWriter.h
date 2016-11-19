#pragma once

namespace Titanium { namespace TIRA
{
	class MemWriter
	{
	public:
		MemWriter(unsigned char *buf, unsigned int size);

		void WriteUINT8(unsigned char x);
		void WriteINT8(char x);
		void WriteUINT16(unsigned short x);
		void WriteINT16(short x);
		void WriteUINT32(unsigned int x);
		void WriteINT32(int x);
		void WriteUINT64(unsigned long long x);
		void WriteINT64(long long x);
	private:
		unsigned char *m_Buf;
		unsigned int m_Size;
		unsigned int m_BytesConsumed;
	};
}
}
