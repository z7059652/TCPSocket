#pragma once
#include <chrono>
#define ti_clock_hnano_to_milli(hnano) (hnano/10000)
#define ti_clock_hnano_to_second(hnano) (hnano/10000000)
#define ti_clock_milli_to_hnano(milli) (milli * 10000)

namespace Titanium { namespace TIRA
{
	struct ti_timestamp
	{
		long long  __hnano;
		inline void set_hnano(long long hnano)
		{
			__hnano = hnano;
		}
		inline void add_hnano(long long hnano)
		{
			__hnano += hnano;
		}
		inline long long as_milli() const
		{
			return ti_clock_hnano_to_milli(__hnano);
		}
		inline long long as_micro() const
		{
			return __hnano / 10;
		}
		inline long long as_hnano() const
		{
			return __hnano;
		}
	};

	class ti_system_clock
	{
	public:
		static long long now_as_milli();
		static long long now_as_micro();
		static long long now_as_hnano();
		static ti_timestamp now_as_timestamp();
	};
}
}
