#pragma once

namespace Titanium {
	namespace TIRA
	{
		typedef enum
		{
			TI_FORMAT_UNKNOWN = 0,
			TI_FORMAT_RGB_24 = 86,
			TI_FORMAT_B8G8R8A8_UNORM = 87,
			TI_FORMAT_U8Y8V8Y8 = 88,
			TI_FORMAT_Y8U8Y8V8 = 90,
			TI_FORMAT_I444 = 92,
			// 
			TI_FORMAT_IYUV = 94,
			TI_FORMAT_NV12 = 96
		} TI_PIXEL_FORMAT;

		typedef struct __ImageDesc
		{
			unsigned int m_Width;
			unsigned int m_Height;
			unsigned int m_Stride;
			bool m_BottomUp;
			TI_PIXEL_FORMAT m_PixelFormat;
			bool operator==(const __ImageDesc &other)const
			{
				return m_Width == other.m_Width
					&& m_Height == other.m_Height
					&& m_PixelFormat == other.m_PixelFormat
					&& m_BottomUp == other.m_BottomUp
					&& m_Stride == other.m_Stride;
			}
			bool operator!=(const __ImageDesc &other) const
			{
				return !(*this == other);
			}
			unsigned int GetImageSizeInBytes() const
			{
				return m_Stride * m_Height;
			}
			static unsigned int GetStride(TI_PIXEL_FORMAT format, unsigned int width)
			{
				unsigned int Bpp = GetBytesPerPixel(format);
				return (((Bpp * width) + 3) >> 2) << 2;
			}
			static unsigned int GetBytesPerPixel(TI_PIXEL_FORMAT format)
			{
				unsigned int bpp = 0;

				switch (format)
				{
				case TI_FORMAT_RGB_24:
					bpp = 24;
					break;
				case TI_FORMAT_B8G8R8A8_UNORM:
					bpp = 32;
					break;
				case TI_FORMAT_U8Y8V8Y8:
					bpp = 16;
					break;
				case TI_FORMAT_Y8U8Y8V8:
					bpp = 16;
					break;
				case TI_FORMAT_I444:
					bpp = 24;
					break;
				}

				return bpp >> 3;
			}
		} ImageDesc;
	}
}
