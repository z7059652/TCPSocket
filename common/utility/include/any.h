#pragma once
#include <type_traits>
#include <memory>

namespace Titanium { namespace TIRA
{
	class any
	{
	public:
		any() {}

		template <typename T>
		any(T const& val)
			: pImpl_(new any_impl<T>(val))
		{
		}

		any(any const& other)
			: pImpl_(other.pImpl_ ? other.pImpl_->clone() : nullptr)
		{
		}

		template <typename T>
		any& operator=(T const& val)
		{
			pImpl_.reset(new any_impl<T>(val));
			return *this;
		}

		any& operator=(any const& other)
		{
			pImpl_.reset(other.pImpl_ ? other.pImpl_->clone() : nullptr);
			return *this;
		}

		bool empty() const
		{
			return pImpl_ == nullptr;
		}

	private:
		class any_base
		{
		public:
			virtual any_base* clone() const = 0;
			virtual ~any_base() {}
		};

		template <typename T>
		class any_impl : public any_base
		{
		public:
			explicit any_impl(T val)
				: val_(val)
			{
				static_assert(!std::is_same<T, any>::value, "any_impl shouldn't store any");
			}

			any_base* clone() const /*override*/
			{
				return new any_impl<T>(val_);
			}

			T val_;
		};

		std::unique_ptr<any_base> pImpl_;

		template <typename T>
		friend T* any_cast(any*);
	};

	template <typename T>
	T* any_cast(any* operand)
	{
		auto p = dynamic_cast<any::any_impl<T>*>(operand->pImpl_.get());
		return p == nullptr ? nullptr : &p->val_;
	}

	class bad_any_cast : public std::bad_cast
	{
	public:
		virtual const char* what() const throw()
		{
			return "any_cast: failed conversion";
		}
	};

	template <typename T>
	T any_cast(any& operand)
	{
		typedef typename std::remove_reference<T>::type noref;

		noref* p = any_cast<noref>(&operand);
		if (p == nullptr)
		{
			throw bad_any_cast();
		}
		return *p;
	}
}
}

