#pragma once
#include "any.h"
#include "ScopeGuard.h"
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <iterator>

namespace Titanium { namespace TIRA
{
	struct ConnectionBase
	{
		ConnectionBase() {}
		ConnectionBase(any iter, std::function<void(any)> fnDisconnect)
			: m_iter(iter), m_fnDisconnect(fnDisconnect)
		{
		}

		std::function<void(any)> m_fnDisconnect;
		any                      m_iter;
	};

	class SignalConnection
	{
	public:
		SignalConnection(std::shared_ptr<ConnectionBase> const& conn)
			: m_conn(conn)
		{
		}

		bool connected() const
		{
			return m_conn && m_conn->m_fnDisconnect;
		}

		void disconnect()
		{
			if (!connected())
			{
				return;
			}

			auto fnDisconn = m_conn->m_fnDisconnect;
			m_conn->m_fnDisconnect = nullptr;
			fnDisconn(m_conn->m_iter);
		}

	private:
		std::shared_ptr<ConnectionBase> m_conn;
	};

	class Scoped_connection
	{
	public:
		Scoped_connection() {}

		Scoped_connection(SignalConnection const& conn)
			: m_conn(new SignalConnection(conn))
		{
		}

		Scoped_connection& operator=(SignalConnection const& conn)
		{
			Disconnect();
			m_conn.reset(new SignalConnection(conn));
			return *this;
		}

		~Scoped_connection()
		{
			Disconnect();
		}
		void Disconnect()
		{
			if (m_conn)
			{
				m_conn->disconnect();
			}
		}

	private:
		std::unique_ptr<SignalConnection> m_conn;

	private: // noncopyable 
		Scoped_connection(Scoped_connection const&);
		Scoped_connection& operator=(Scoped_connection const&);
	};

	typedef std::pair<any, std::shared_ptr<ConnectionBase>> TSlot;
	typedef std::multimap<int, TSlot>                       TSlotStorage;
	typedef TSlotStorage::iterator                          TSlotIter;

	class SlotManager
	{
	public:
		SlotManager()
		{
		}

		void disconnect(TSlotIter iter)
		{
			m_storage.erase(iter);
		}

		unsigned int size() const
		{
			return (unsigned int)m_storage.size();
		}


		std::shared_ptr<ConnectionBase> insert(int key, any slot)
		{
			auto conn = std::make_shared<ConnectionBase>();
			auto iter = m_storage.insert(TSlotStorage::value_type(key, TSlot(slot, conn)));
			conn->m_fnDisconnect = [=](any iter) { disconnect(any_cast<TSlotIter>(iter)); };
			conn->m_iter = iter;
			return conn;
		}

		void ForEach(std::function<void(any&)> func)
		{
			// extra copy, so that the invocation chain can be finished even if the slot manager
			// gets destroyed during		
			std::vector<any> slots;
			std::transform(m_storage.begin(), m_storage.end(), std::back_inserter(slots),
				[](TSlotStorage::reference slot) { return slot.second.first; });

			std::for_each(slots.begin(), slots.end(), [&](any& slot)
			{
				func(slot);
			});
		}

		void FIND_IF(std::function<bool(any&)> func)
		{
			std::vector<any> slots;
			std::transform(m_storage.begin(), m_storage.end(), std::back_inserter(slots),
				[](TSlotStorage::reference slot) { return slot.second.first; });

			std::find_if(slots.begin(), slots.end(), [&](any& slot)
			{
				return func(slot);
			});
		}

		~SlotManager()
		{
			std::for_each(m_storage.begin(), m_storage.end(), [&](TSlotStorage::reference& slot)
			{
				slot.second.second->m_fnDisconnect = nullptr;
			});
		}
	protected:
		TSlotStorage m_storage;
	};
	
	template <typename Sig>
	class SignalBase : public SlotManager
	{
	public:
		SignalConnection connect(std::function<Sig> callback)
		{
			return SignalConnection(insert(-1, callback));
		}
	};

	template <typename S>
	class Signal;

	template <>
	class Signal<void()>: public SignalBase < void() >
	{
	public:
		void operator()()
		{
			typedef std::function<void()> TCallback;
			this->ForEach([&](any& slot)
			{
				any_cast<TCallback&>(slot)();
			});
		}
	};

	template <typename T1>
	class Signal<void(T1)>: public SignalBase < void(T1) >
	{
	public:
		void operator()(T1 a1)
		{
			typedef std::function<void(T1)> TCallback;
			this->ForEach([&](any& slot)
			{
				any_cast<TCallback&>(slot)(a1);
			});
		}
	};

	template <typename T1, typename T2>
	class Signal<void(T1, T2)>: public SignalBase < void(T1, T2) >
	{
	public:
		void operator()(T1 a1, T2 a2)
		{
			typedef std::function<void(T1, T2)> TCallback;
			this->ForEach([&](any& slot)
			{
				any_cast<TCallback&>(slot)(a1, a2);
			});
		}
	};

	template <typename T1, typename T2, typename T3>
	class Signal<void(T1, T2, T3)>: public SignalBase < void(T1, T2, T3) >
	{
	public:
		void operator()(T1 a1, T2 a2, T3 a3)
		{
			typedef std::function<void(T1, T2, T3)> TCallback;
			this->ForEach([&](any& slot)
			{
				any_cast<TCallback&>(slot)(a1, a2, a3);
			});
		}
	};

	template <typename T1, typename T2, typename T3, typename T4>
	class Signal<void(T1, T2, T3, T4)>: public SignalBase < void(T1, T2, T3, T4) >
	{
	public:
		void operator()(T1 a1, T2 a2, T3 a3, T4 a4)
		{
			typedef std::function<void(T1, T2, T3, T4)> TCallback;
			this->ForEach([&](any& slot)
			{
				any_cast<TCallback&>(slot)(a1, a2, a3, a4);
			});
		}
	};
}
}
