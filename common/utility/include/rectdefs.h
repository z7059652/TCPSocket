#pragma once

namespace Titanium { namespace TIRA
{
#ifdef __GNUC__
	struct RECT
	{
		long left;
		long top;
		long right;
		long bottom;
	};
#endif
	typedef struct __TI_RECT_SIZE
	{
		unsigned int m_Width;
		unsigned int m_Height;
		bool IsZeroSize()
		{
			return m_Width == 0
				&& m_Height == 0;
		}
		static __TI_RECT_SIZE ZeroSize()
		{
			__TI_RECT_SIZE rt = { 0, 0 };
			return rt;
		}
		bool operator == (const __TI_RECT_SIZE & other) const
		{
			return other.m_Height == this->m_Height && other.m_Width == this->m_Width;
		}

		unsigned int Area()const
		{
			return m_Width * m_Height;
		}


	}TI_RECT_SIZE, TI_Resolution;

#define RECTWIDTH(x) ((x).right - (x).left)   
#define RECTHEIGHT(x) ((x).bottom - (x).top) 
#define RECT_CONTAINS(rectC, testME)	\
(rectC.left <= testME.left && rectC.top <= testME.top	&& rectC.right >= testME.right && rectC.bottom >= testME.bottom)

}
}
