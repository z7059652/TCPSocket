#pragma once
#include <list>
#include <algorithm>

namespace Titanium { namespace TIRA
{
	template <typename T>
	class Trimmean
	{
	public:
		Trimmean(int size = 0, T val = 0)
		{
			m_List = std::list<T>(size, val);
		}

		T GetMean()
		{
			std::lock_guard<std::mutex> lock(m_Mtx);
			if (m_List.empty())
				return 0;

			T sum = 0;
			for (T val : m_List)
				sum += val;

			if (m_List.size() <= 2)
				return sum / m_List.size();

			T maxVal = *std::max_element(m_List.begin(), m_List.end());
			T minVal = *std::min_element(m_List.begin(), m_List.end());
			return (sum - maxVal - minVal) / (m_List.size() - 2);
		}

		void Push(T val)
		{
			std::lock_guard<std::mutex> lock(m_Mtx);
			m_List.pop_front();
			m_List.push_back(val);
		}

	private:
		std::list<T> m_List;
		std::mutex m_Mtx;
	};
}
}
