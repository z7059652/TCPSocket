#pragma once
#include "error_handling_utility.h"

namespace Titanium { namespace TIRA
{
	template <class T>
	class ComSmartPtr
	{
	public:
		ComSmartPtr(): p(nullptr) {}

		ComSmartPtr(T* lp): p(lp) { AddRefInternal(); }

		ComSmartPtr(const ComSmartPtr<T>& lp)
		{
			p = lp.p;
			AddRefInternal();
		}

		~ComSmartPtr() { ReleaseInternal(); }

		operator T*() const { return p; }

		T& operator*() const { ENSURE(p != nullptr); return *p; }


		T** operator&() { ENSURE(p == nullptr); return &p; }

		T* operator->() const
		{
			ENSURE(p != nullptr);
			return p;
		}

		T* operator=(T* lp)
		{
			if (lp == p)
			{
				return p;
			}

			ReleaseInternal();
			p = lp;
			AddRefInternal();
			return p;
		}

		T* operator=(const ComSmartPtr<T>& lp)
		{
			if (lp.p == p)
			{
				return p;
			}

			ReleaseInternal();
			p = lp.p;
			AddRefInternal();
			return p;
		}

		bool operator!() const { return p == nullptr; }

		bool operator<(T* pT) const { return p < pT; }

		bool operator==(T* pT) const { return p == pT; }

		bool operator!=(T* pT) const { return p != pT; }

		void Attach(T* p2) { ReleaseInternal(); p = p2; }

		T* Detach()
		{
			T* pt = p;
			p = nullptr;
			return pt;
		}
	private:

		void AddRefInternal()
		{
			if (p != nullptr)
			{
				p->AddRef();
			}
		}

		void ReleaseInternal()
		{
			T* pTemp = p;
			if (pTemp)
			{
				p = nullptr;
				pTemp->Release();
			}
		}

	protected:
		T* p;

	};
}
}
