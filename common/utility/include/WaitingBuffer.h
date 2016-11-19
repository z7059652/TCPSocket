#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "error_handling_utility.h"
#include "ScopeGuard.h"

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace Titanium { namespace TIRA
{
	template<typename value_type>
	class WaitingBuffer
	{
	public:
		WaitingBuffer(unsigned int size)
			: m_Start(0)
			, m_End(0)
			, m_Size(size)
			, m_DataLen(0)
			, m_IsCancelRead(false)
			, m_IsCancelWait(false)
			, m_IsCancelWrite(false)
		{
			m_Buffer.reset(new value_type[size]);
		}
		virtual ~WaitingBuffer()
		{
			CancelAllOperation();
		}

	public:
		void WaitTillBufferEmpty()
		{
			std::unique_lock<std::mutex> cvLock(m_BufMtx);
			ON_SCOPE_EXIT([&]()
			{
				m_IsCancelWait = false;
			});
			m_WaitCV.wait(cvLock, [&]()
			{
				return m_DataLen == 0 || m_IsCancelWait == true;
			});
			if (m_IsCancelWait)
				throw ExceptionWithString("WaitingBuffer has been canceled.");
		}
		void WriteTillFinish(const value_type* buffer, unsigned int len)
		{
			std::lock_guard<std::mutex> writeLock(m_WriteMtx);
			ON_SCOPE_EXIT([&]()
			{
				m_IsCancelWrite = false;
			});
			while (len > 0 && !m_IsCancelWrite)
			{
				std::unique_lock<std::mutex> writeCVLock(m_BufMtx);
				if (m_IsCancelWrite)
				{
					writeCVLock.unlock();
					throw ExceptionWithString("Writing is Canceled!");
				}
				if (m_Size - m_DataLen > 0)
				{
					unsigned int writenLen = WriteInternal(buffer, len);
					buffer += writenLen;
					len -= writenLen;
					writeCVLock.unlock();
					m_ReadCV.notify_all();
				}
				else
				{
					//m_BufMtx.unlock();
					m_WriteCV.wait(writeCVLock);
				}
			}
		}
		void ReadTilFinish(value_type* buffer, unsigned int len)
		{
			ReadAtless(buffer, len, len);
		}
		unsigned int ReadAtless(value_type* buffer, unsigned int len, unsigned int atless)
		{
			std::lock_guard<std::mutex> readLock(m_ReadMtx);
			ON_SCOPE_EXIT([&]()
			{
				m_IsCancelRead = false;
			});

			int readCount = 0;
			while ((int)atless > readCount)
			{
				std::unique_lock<std::mutex> readCVLock(m_BufMtx);
				if (m_IsCancelRead)
				{
					readCVLock.unlock();
					throw ExceptionWithString("WaitingBuffer: Reading is canceled!");
				}
				if (m_DataLen > 0)
				{
					unsigned int readLen = ReadInternal(buffer, len);
					buffer += readLen;
					readCount += readLen;
					len -= readLen;
					readCVLock.unlock();
					m_WriteCV.notify_all();
					m_WaitCV.notify_all();
				}
				else
				{
					//m_BufMtx.unlock();
					m_ReadCV.wait(readCVLock);
				}
			}

			return readCount;
		}
		void CancelAllOperation()
		{
			CancelRead();
			CancelWrite();
			CancelWait();
		}
		void CancelRead()
		{
			std::lock_guard<std::mutex> lock(m_BufMtx);
			m_IsCancelRead = true;
			m_ReadCV.notify_all();
		}
		void CancelWrite()
		{
			std::lock_guard<std::mutex> lock(m_BufMtx);
			m_IsCancelWrite = true;
			m_WriteCV.notify_all();
		}

		void CancelWait()
		{
			std::lock_guard<std::mutex> lock(m_BufMtx);
			m_IsCancelWait = true;
			m_WaitCV.notify_all();
		}

		unsigned int DataLen()
		{
			std::lock_guard<std::mutex> lock(m_BufMtx);
			return m_DataLen;
		}
	protected:
		int WriteInternal(const value_type* buffer, unsigned int len)
		{
			unsigned int writeLen = MIN(m_Size - m_DataLen, len);

			if (m_End + writeLen <= m_Size)
			{
				memcpy(m_Buffer.get() + m_End, buffer, writeLen);
			}
			else
			{
				unsigned int rightLen = m_Size - m_End;
				unsigned int leftLen = writeLen - rightLen;

				memcpy(m_Buffer.get() + m_End, buffer, rightLen);
				buffer += rightLen;
				memcpy(m_Buffer.get(), buffer, leftLen);
			}

			m_End = (m_End + writeLen) % m_Size;
			m_DataLen += writeLen;
			return writeLen;
		}

		int ReadInternal(value_type* buffer, unsigned int len)
		{
			unsigned int readLen = MIN((unsigned int)m_DataLen, len);

			if (m_Start + readLen <= m_Size)
			{
				memcpy(buffer, m_Buffer.get() + m_Start, readLen);
			}
			else
			{
				unsigned int rightLen = m_Size - m_Start;
				unsigned int leftLen = readLen - rightLen;

				memcpy(buffer, m_Buffer.get() + m_Start, rightLen);
				buffer += rightLen;

				memcpy(buffer, m_Buffer.get(), leftLen);
			}

			m_Start = (m_Start + readLen) % m_Size;
			m_DataLen = m_DataLen - readLen;

			return readLen;
		}

	private:
		std::unique_ptr<value_type[]> m_Buffer;
		unsigned int m_Start;
		unsigned int m_End;
		unsigned int m_Size;
		unsigned int m_DataLen;

		std::mutex m_BufMtx;
		std::mutex m_ReadMtx;
		std::mutex m_WriteMtx;
		std::mutex m_WaitMtx;
		std::condition_variable m_WriteCV;
		std::condition_variable m_ReadCV;
		std::condition_variable m_WaitCV;
		bool m_IsCancelRead;
		bool m_IsCancelWait;
		bool m_IsCancelWrite;
	};
}
}
