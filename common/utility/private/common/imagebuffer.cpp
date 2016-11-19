#include "imagebuffer.h"
#include "error_handling_utility.h"
#ifdef WIN32
#include "ImageProcess.h"
#endif
using namespace Titanium::TIRA;

const ImageDesc & Ti_Image::GetImageDesc() const
{
	return m_ImageDesc;
}

void Ti_Image::CopyFrom(const unsigned char* pData, const ImageDesc& srcDesc, const RECT& pSrcRect)
{
	if (!((unsigned int)pSrcRect.left >= 0 && (unsigned int)pSrcRect.top >= 0
		&& (unsigned int)pSrcRect.right <= srcDesc.m_Width && (unsigned int)pSrcRect.bottom <= srcDesc.m_Height
		&& (unsigned int)pSrcRect.right <= m_ImageDesc.m_Width && (unsigned int)pSrcRect.bottom <= m_ImageDesc.m_Height))
		return;

	if (srcDesc.m_PixelFormat == m_ImageDesc.m_PixelFormat)
	{
		if (m_ImageDesc.m_PixelFormat == TI_FORMAT_I444)
		{
			unsigned int lineLen = pSrcRect.right - pSrcRect.left;
			const unsigned char* pSrcY = pData + ((unsigned int)pSrcRect.left + (unsigned int)pSrcRect.top * srcDesc.m_Width);
			const unsigned char* pSrcU = pSrcY + srcDesc.m_Width*srcDesc.m_Height;
			const unsigned char* pSrcV = pSrcU + srcDesc.m_Width*srcDesc.m_Height;
			unsigned char* pDstY = m_InternalBuffer->GetPtrForWriting() + (pSrcRect.top * m_ImageDesc.m_Width + pSrcRect.left);
			unsigned char* pDstU = pDstY + m_ImageDesc.m_Width*m_ImageDesc.m_Height;
			unsigned char* pDstV = pDstU + m_ImageDesc.m_Width*m_ImageDesc.m_Height;
			//full frame copy.
			if (pSrcRect.top == 0 && pSrcRect.left == 0
				&& pSrcRect.right == m_ImageDesc.m_Width&& pSrcRect.bottom == m_ImageDesc.m_Height
				&& pSrcRect.right == srcDesc.m_Width&& pSrcRect.bottom == srcDesc.m_Height
				&& srcDesc.m_Stride == m_ImageDesc.m_Stride)
			{
				memcpy(pDstY, pSrcY, m_ImageDesc.m_Stride * m_ImageDesc.m_Height);
				return;
			}
			if (pSrcRect.top == 0 && pSrcRect.left == 0
				&& pSrcRect.right == m_ImageDesc.m_Width&& !(pSrcRect.bottom > m_ImageDesc.m_Height)
				&& pSrcRect.right == srcDesc.m_Width&& pSrcRect.bottom == srcDesc.m_Height
				&& srcDesc.m_Stride == m_ImageDesc.m_Stride)
			{
				memcpy(pDstY, pSrcY, pSrcRect.right * pSrcRect.bottom);
				memcpy(pDstU, pSrcU, pSrcRect.right * pSrcRect.bottom);
				memcpy(pDstV, pSrcV, pSrcRect.right * pSrcRect.bottom);
				return;
			}
			for (int i = pSrcRect.top; i < pSrcRect.bottom; i++)
			{
				memcpy(pDstY, pSrcY, lineLen);
				memcpy(pDstU, pSrcU, lineLen);
				memcpy(pDstV, pSrcV, lineLen);
				pDstY += m_ImageDesc.m_Width;
				pSrcY += srcDesc.m_Width;
				pDstU += m_ImageDesc.m_Width;
				pSrcU += srcDesc.m_Width;
				pDstV += m_ImageDesc.m_Width;
				pSrcV += srcDesc.m_Width;
			}
		}
		else
		{
			unsigned int Bpp = ImageDesc::GetBytesPerPixel(srcDesc.m_PixelFormat);
			//quick implementation given above assumesion.

			unsigned int lineLen = (pSrcRect.right - pSrcRect.left)*Bpp;
			const unsigned char* pSrc = pData + ((unsigned int)pSrcRect.left * Bpp + (unsigned int)pSrcRect.top * srcDesc.m_Stride);
			unsigned char* pDst = m_InternalBuffer->GetPtrForWriting() + (pSrcRect.top *m_ImageDesc.m_Stride + pSrcRect.left* Bpp);

			//full frame copy.
			if (pSrcRect.top == 0 && pSrcRect.left == 0
				&& pSrcRect.right == m_ImageDesc.m_Width&& pSrcRect.bottom == m_ImageDesc.m_Height
				&& pSrcRect.right == srcDesc.m_Width&& pSrcRect.bottom == srcDesc.m_Height
				&& srcDesc.m_Stride == m_ImageDesc.m_Stride)
			{
				memcpy(pDst, pSrc, lineLen*m_ImageDesc.m_Height);
				return;
			}

			for (int i = pSrcRect.top; i < pSrcRect.bottom; i++)
			{
				memcpy(pDst, pSrc, lineLen);
				pDst += m_ImageDesc.m_Stride;
				pSrc += srcDesc.m_Stride;
			}
		}
	}
	else
	{
#ifdef WIN32
		ENSURE(srcDesc.m_PixelFormat == TI_FORMAT_B8G8R8A8_UNORM && m_ImageDesc.m_PixelFormat == TI_FORMAT_I444)("Now we only support BGRA to I444.")(srcDesc.m_PixelFormat)(m_ImageDesc.m_PixelFormat);

		unsigned int BppSrc = ImageDesc::GetBytesPerPixel(srcDesc.m_PixelFormat);
		unsigned int left = pSrcRect.left;
		unsigned int right = pSrcRect.right;
		if ((right - left) % 16 != 0)
		{
			if (left >= (16 - (right - left) % 16))
				left -= (16 - (right - left) % 16);
			else if ((m_ImageDesc.m_Width - right) >= (16 - (right - left) % 16))
				right += (16 - (right - left) % 16);
			else
			{
				left = 0;
				right = m_ImageDesc.m_Width;
			}
		}
		const unsigned char* pSrc = pData + ((unsigned int)left * BppSrc + (unsigned int)pSrcRect.top * srcDesc.m_Stride);
		unsigned char* pDstY = m_InternalBuffer->GetPtrForWriting() + (pSrcRect.top * m_ImageDesc.m_Width + left);
		unsigned char* pDstU = pDstY + m_ImageDesc.m_Width*m_ImageDesc.m_Height;
		unsigned char* pDstV = pDstU + m_ImageDesc.m_Width*m_ImageDesc.m_Height;

		ImageProcess::BGRA32ToYUV444_SSE((unsigned char*)pSrc, srcDesc.m_Stride, pDstY, pDstU, pDstV, m_ImageDesc.m_Width, (right - left), (pSrcRect.bottom - pSrcRect.top));
#else
		throw ExceptionWithString(L"Unimplemented!");
#endif
	}
}

unsigned char* Ti_Image::Lock()
{
	return m_InternalBuffer->GetPtrForWriting();
}

const unsigned char* Ti_Image::GetPrt(unsigned x, unsigned y) const
{
	return m_InternalBuffer->GetPtrForReading() + (y *m_ImageDesc.m_Stride + x* ImageDesc::GetBytesPerPixel(m_ImageDesc.m_PixelFormat));
}

void Ti_Image::Setup(const ImageDesc& desc, std::shared_ptr<DataBuffer<unsigned char>> buffer)
{
	m_InternalBuffer = buffer;
	Setup(desc);
}

void Ti_Image::Setup(unsigned int width, unsigned int height, TI_PIXEL_FORMAT pf)
{
	ImageDesc desc = { width, height, ImageDesc::GetStride(pf, width), true, pf };
	Setup(desc);
}

void Ti_Image::Setup(const ImageDesc& desc)
{
	m_ImageDesc = desc;
	unsigned int newBufferSize = m_ImageDesc.m_Stride * m_ImageDesc.m_Height;
	if (desc.m_PixelFormat == TI_FORMAT_IYUV || desc.m_PixelFormat == TI_FORMAT_NV12)
	{
		newBufferSize = m_ImageDesc.m_Width*m_ImageDesc.m_Height * 3 / 2;
	}

	m_InternalBuffer->SetBufferSizeLowerBound(newBufferSize);
}

Ti_Image::~Ti_Image()
{

}

Ti_Image::Ti_Image()
{
	memset(&m_ImageDesc, 0, sizeof(m_ImageDesc));
	m_InternalBuffer.reset(new DataBuffer<unsigned char>());
}