#pragma once

#include <type_traits>
#include <string>
#include <sstream>
#include <map>
#include "exception.h"
#include "ScopeGuard.h"

namespace Titanium { namespace TIRA 
{
	typedef std::map<std::wstring, std::wstring> TContextVarMap;
	// format message
	std::wstring FormatMessage(
		std::wstring const& expr,
		TContextVarMap const& contextVars,
		std::wstring const& file,
		long line);

	// EnsureBase
	class EnsureBase
	{
	public:
		EnsureBase(std::wstring expr)
			: expr_(expr)
		{}
	protected:
		std::wstring expr_;
		std::wstring file_;
		long line_;
		TContextVarMap contextVars_;
	};

#pragma warning(push)
#pragma warning(disable: 4355)

	// Ensure
	template <typename TBehavior>
	class Ensure : private EnsureBase
	{
	public:
		Ensure(std::wstring expr)
			: EnsureBase(std::move(expr)),
			ENSURE_A(*this),
			ENSURE_B(*this),
			abandoned_(false)
		{}

		Ensure(Ensure const& other)
			: EnsureBase(other),
			ENSURE_A(*this),
			ENSURE_B(*this),
			abandoned_(other.abandoned_)
		{
			other.abandoned_ = true;
		}

		template <typename T>
		Ensure& PrintContextVar(std::wstring const& name, T&& val)
		{
			contextVars_[name] = ToWString(val);
			return *this;
		}

		Ensure& PrintLocation(std::wstring file, long line)
		{
			file_ = std::move(file);
			line_ = line;
			return *this;
		}

		~Ensure()
		{
			if (abandoned_)
			{
				return;
			}

			TBehavior()(expr_, contextVars_, file_, line_);
		}

		Ensure& ENSURE_A;
		Ensure& ENSURE_B;

	private:
		mutable bool abandoned_;

		template <typename T>
		std::wstring ToWString(T&& t)
		{
			std::wstringstream wss;
			wss << t;
			return wss.str();
		}
	};
#pragma warning(pop)


	// Behaviors
	struct ThrowBehavior
	{
		void operator()(
			std::wstring const& expr,
			TContextVarMap const& contextVars,
			std::wstring const& file,
			long line);
	};

	struct AssertBehavior
	{
		void operator()(
			std::wstring const& expr,
			TContextVarMap const& contextVars,
			std::wstring const& file,
			long line);
	};

	struct ThrowWithoutDumpBehavior
	{
		void operator()(
			std::wstring const& expr,
			TContextVarMap const& contextVars,
			std::wstring const& file,
			long line);
	};

	struct TraceBehavior
	{
		void operator()(
			std::wstring const& expr,
			TContextVarMap const& contextVars,
			std::wstring const& file,
			long line);
	};

	struct WarningBehavior
	{
		void operator()(
			std::wstring const& expr,
			TContextVarMap const& contextVars,
			std::wstring const& file,
			long line);
	};

	inline Ensure<WarningBehavior> MakeEnsureWarning(std::wstring expr)
	{
		return Ensure<WarningBehavior>(std::move(expr));
	}

	inline Ensure<ThrowBehavior> MakeEnsureThrow(std::wstring expr)
	{
		return Ensure<ThrowBehavior>(std::move(expr));
	}

	inline Ensure<ThrowWithoutDumpBehavior> MakeEnsureThrowWithoutDump(std::wstring expr)
	{
		return Ensure<ThrowWithoutDumpBehavior>(std::move(expr));
	}

	inline Ensure<TraceBehavior> MakeTrace(std::wstring expr)
	{
		return Ensure<TraceBehavior>(std::move(expr));
	}

	inline Ensure<AssertBehavior> MakeEnsureAssert(std::wstring expr)
	{
		return Ensure<AssertBehavior>(std::move(expr));
	}
}
}
