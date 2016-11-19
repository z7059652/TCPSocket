#pragma once
#include <memory>
#include "error_handling_utility.h"

#define TI_SWAP(A, B) {auto TI_SWAP_TEMP = A; A = B; B = TI_SWAP_TEMP;}

namespace Titanium { namespace TIRA
{
	template<typename BaseDataType>
	class Buffer
	{
	public:
		Buffer():
			m_BufferSize(0),
			m_IsOwnResource(true),
			m_Buffer(nullptr)
		{
		}
		Buffer(std::shared_ptr<BaseDataType> buffer, unsigned int bufferSize):
			m_BufferHolder(buffer),
			m_Buffer(buffer.get()),
			m_BufferSize(bufferSize),
			m_IsOwnResource(true)
		{
		}
		Buffer(BaseDataType* buffer, unsigned int bufferSize):
			m_Buffer(buffer),
			m_BufferSize(bufferSize),
			m_IsOwnResource(false)
		{
		}
	public:
		inline void SetBufferSizeLowerBound(unsigned int newSize)
		{
			if (newSize > m_BufferSize)
			{
				ENSURE(m_IsOwnResource)("Not allow extending for external buffer!");
				m_BufferSize = newSize;
				m_BufferHolder.reset(new BaseDataType[m_BufferSize], std::default_delete<BaseDataType[]>());
				m_Buffer = m_BufferHolder.get();
			}
		}
		inline unsigned int GetBufferSize()const { return m_BufferSize; }
		inline const BaseDataType* GetPtrForReading(unsigned int position = 0)const
		{
			if (m_BufferSize == 0)
				return nullptr;

			ENSURE(sizeof(BaseDataType) * position < m_BufferSize);
			return m_Buffer + position;
		}
		inline BaseDataType* GetPtrForWriting(unsigned int position = 0)
		{
			ENSURE(sizeof(BaseDataType) * position < m_BufferSize);
			return m_Buffer + position;
		}
	public:
		template<typename DataType>
		inline void SetBufferSizeLowerBound(unsigned int newSize)
		{
			SetBufferSizeLowerBound((newSize * sizeof(DataType)) / sizeof(BaseDataType) + 1);
		}

		template<typename DataType>
		inline unsigned int GetBufferSize()const { return (m_BufferSize * sizeof(BaseDataType)) / sizeof(DataType); }

		template<typename DataType>
		inline const DataType* GetPtrForReading(unsigned int position = 0)const
		{
			ENSURE(sizeof(DataType) * position < m_BufferSize);
			return ((DataType*)m_Buffer) + position;
		}

		template<typename DataType>
		inline DataType* GetPtrForWriting(unsigned int position = 0)
		{
			ENSURE(sizeof(DataType) * position < m_BufferSize);
			return ((DataType*)m_Buffer) + position;
		}
	public:

		void Swap(Buffer<BaseDataType> *other)
		{
			ENSURE((m_IsOwnResource && other->m_IsOwnResource));
			m_BufferHolder.swap(other->m_BufferHolder);
			TI_SWAP(m_Buffer, other->m_Buffer);
			TI_SWAP(m_BufferSize, other->m_BufferSize);
		}
	protected:
		unsigned int m_BufferSize;
		bool m_IsOwnResource;
		std::shared_ptr<BaseDataType> m_BufferHolder;
		BaseDataType* m_Buffer;
	};
}
}
