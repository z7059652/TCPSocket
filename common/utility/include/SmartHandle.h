#pragma once
#include "target_os.h"
#include "error_handling_utility.h"
#include <memory>
#include <functional>
#ifdef __GNUC__
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET 0
#define SOCKET_ERROR (-1)
#define WSAENOTSOCK (0)
#define closesocket close
#define WSAGetLastError() 0
#endif

namespace Titanium { namespace TIRA
{
	template <typename HandleT>
	struct HandleReleaser;

	template <>
	struct HandleReleaser < SOCKET >
	{
		void operator()(SOCKET sockfd) const
		{
			if (closesocket(sockfd) == SOCKET_ERROR)
			{
				auto error = (WSAGetLastError());
				if (error != WSAENOTSOCK)
				{
					TRACE("error during socket closing: %d", error);
				}
			}
		}
	};

	template <typename HandleT>
	struct HandleVerifier;

	template <>
	struct HandleVerifier < SOCKET >
	{
		bool operator()(SOCKET hsocket) const
		{
			return hsocket != INVALID_SOCKET;
		}
	};

	template <typename HandleT, typename Releaser>
	struct HandleOwner
	{
		HandleOwner(HandleT handle)
			: handle_(handle)
		{
		}

		~HandleOwner()
		{
			Releaser()(handle_);
		}

		HandleT GetHandle()
		{
			return handle_;
		}

	private:
		HandleT handle_;

	private:
		HandleOwner(HandleOwner const&);
		HandleOwner& operator=(HandleOwner const&);
	};

	template <typename HandleT, typename Releaser = HandleReleaser<HandleT>, typename Verifier = HandleVerifier<HandleT>>
	class SmartHandle : public std::shared_ptr < HandleOwner<HandleT, Releaser> >
	{
		typedef HandleOwner<HandleT, Releaser> owner_t;
		typedef std::shared_ptr<owner_t> base_t;

	public:
		SmartHandle() {}
		explicit SmartHandle(HandleT handle)
		{
			ENSURE_WIN32(Verifier()(handle));
			this->reset(new owner_t(handle));
		}

		HandleT get() const
		{
			return base_t::get()->GetHandle();
		}
	};
}
}
