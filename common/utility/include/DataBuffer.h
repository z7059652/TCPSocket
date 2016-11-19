#pragma once
#include "buffer.h"

namespace Titanium { namespace TIRA
{
	template<typename BaseDataType>
	class DataBuffer :public Buffer < BaseDataType >
	{
	public:
		DataBuffer()
			: Buffer<BaseDataType>()
			, m_DataLength(0)
		{
		}
		DataBuffer(std::shared_ptr<BaseDataType> buffer, unsigned int bufferSize):Buffer<BaseDataType>(buffer, bufferSize), m_DataLength(bufferSize) {}
		DataBuffer(BaseDataType* buffer, unsigned int bufferSize):Buffer<BaseDataType>(buffer, bufferSize), m_DataLength(bufferSize) {}
	public:
		void AppendFrom(const BaseDataType* pSrc, unsigned int len)
		{
			ENSURE(len + m_DataLength <= this->m_BufferSize)
				("Not enought space to store appending data.")
				(this->m_BufferSize)
				(m_DataLength)
				(len);

			BaseDataType* pTemp = this->m_Buffer + m_DataLength;
			unsigned int lenToCopy = len * sizeof(BaseDataType);

			memcpy(pTemp, pSrc, lenToCopy);

			m_DataLength += len;
		}
		void CopyFrom(const BaseDataType* pSrc, unsigned int len)
		{
			ENSURE(len <= this->m_BufferSize);
			BaseDataType* pTemp = this->m_Buffer;
			unsigned int lenToCopy = len * sizeof(BaseDataType);
			memcpy(pTemp, pSrc, lenToCopy);
			m_DataLength = len;

		}
		void AppendFrom(const DataBuffer<BaseDataType>& src)
		{
			AppendFrom(src.GetPtrForReading(), src.GetDataLength());
		}
		void CopyFrom(const DataBuffer<BaseDataType>& src)
		{
			CopyFrom(src.GetPtrForReading(), src.GetDataLength());
		}

		inline unsigned int GetSpaceLeave() const
		{
			return this->m_BufferSize - m_DataLength;
		}
		inline unsigned int GetDataLength() const { return m_DataLength; }
		inline void SetDataLength(unsigned int length)
		{
			ENSURE(length <= this->m_BufferSize);
			m_DataLength = length;
		}

		inline void Clear()
		{
			m_DataLength = 0;
		}
	public:

		template<typename DataType>
		void AppendFrom(const DataType* pSrc, unsigned int dataLen)
		{
			AppendFrom((const BaseDataType*)pSrc, dataLen * sizeof(DataType) / sizeof(BaseDataType));
		}

		template<typename DataType>
		void CopyFrom(const DataType* pSrc, unsigned int  dataLen)
		{
			CopyFrom((const BaseDataType*)pSrc, dataLen * sizeof(DataType) / sizeof(BaseDataType));
		}

		template<typename DataType>
		void AppendFrom(const DataBuffer<DataType>& src)
		{
#ifdef __GNUC__
			AppendFrom<unsigned char>(src.GetPtrForReading(), src.GetDataLength<unsigned char>());
#else
			AppendFrom<unsigned char>(src.GetPtrForReading<unsigned char>(), src.GetDataLength<unsigned char>());
#endif //#ifdef __GNUC__
		}

		template<typename DataType>
		void CopyFrom(const DataBuffer<DataType>& src)
		{
#ifdef __GNUC__
			CopyFrom<unsigned char>(src.GetPtrForReading(), src.GetDataLength<unsigned char>());
#else
			CopyFrom<unsigned char>(src.GetPtrForReading<unsigned char>(), src.GetDataLength<unsigned char>());
#endif //#ifdef __GNUC__
		}

		template<typename DataType>
		inline unsigned int GetDataLength() const { return m_DataLength * sizeof(BaseDataType) / sizeof(DataType); }

		template<typename DataType>
		inline void SetDataLength(unsigned int length)
		{
			SetDataLength(length * sizeof(DataType) / sizeof(BaseDataType));
		}
	protected:
		unsigned int m_DataLength;
	};
}
}
