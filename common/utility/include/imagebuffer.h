//      Utility class that provides image buffer management and basic image manipulation functions.
#pragma once
#include <memory>
#include "imagedefs.h"
#include "rectdefs.h"
#include "DataBuffer.h"

namespace Titanium { namespace TIRA
{
	class Ti_Image
	{
	public:
		Ti_Image();
		~Ti_Image();
	public:
		void Setup(const ImageDesc& desc);
		void Setup(unsigned int width, unsigned int height, TI_PIXEL_FORMAT pf);
		void Setup(const ImageDesc& desc, std::shared_ptr<DataBuffer<unsigned char>> buffer);
		const unsigned char* GetPrt(unsigned x, unsigned y)	const;
		unsigned char* Lock();
		void CopyFrom(const unsigned char* pData, const ImageDesc& srcDesc, const RECT& pSrcRect);
		const ImageDesc & GetImageDesc() const;
	private:
		ImageDesc m_ImageDesc;
		std::shared_ptr<DataBuffer<unsigned char>> m_InternalBuffer;
	};
}
}
