#pragma once

#include "Runtime/Core/Definitions.h"

template<class... Args>
class Event;

template<class... Args>
class EventHandler;

template<class... Args>
class Event final
{
public:
	using Callback = std::function<void(Args...)>;
	using HandlerType = EventHandler<Args...>;

	Event() = default;

	Event(Event&& Other)
		: m_Handlers(std::move(Other.m_Handlers))
	{
		RebindAll();
	}

	Event& operator=(Event&& Other)
	{
		UnbindAll();

		m_Handlers = std::move(Other.m_Handlers);

		RebindAll();

		return *this;
	}

	~Event()
	{
		UnbindAll();
	}

	bool8_t HasAnyBind() const
	{
		for (HandlerType* Handler : m_Handlers)
		{
			if (Handler)
			{
				return true;
			}
		}

		return false;
	}

	void UnbindAll()
	{
		for (HandlerType* Handler : m_Handlers)
		{
			assert(Handler->m_Event == this);
			Handler->Unbind();
		}
		m_Handlers.clear();
	}

	void Bind(HandlerType& Handler) const
	{

	}

	void UnBind(HandlerType& Handler) const
	{

	}

	void Signal(const Args&... ArgList) const
	{

	}
protected:

	void RebindAll()
	{
		for (HandlerType* Handler : m_Handlers)
		{
			if (Handler)
			{
				assert(Handler->m_Event != this);
				Handler->m_Event = this;
			}
		}
	}
private:
	friend class EventHandler<Args...>;
	mutable std::vector<HandlerType*> m_Handlers;
};

template<class... Args>
class EventHandler final
{
public:
	using Callback = std::function<void(Args...)>;
	using EventType = Event<Args...>;

	EventHandler() = default;

	explicit EventHandler(std::nullptr_t)
	{
	}

	explicit EventHandler(Callback Callback)
		: m_Callback(std::move(Callback))
	{
	}

	EventHandler(const EventHandler& Other)
		: m_Callback(Other.m_Callback)
		, m_Event(Other.m_Event)
	{
		if (m_Callback && m_Event)
		{
			m_Event->Bind(*this);
		}
		else
		{
			m_Event = nullptr;
		}
	}

	EventHandler(EventHandler&& Other)
		: m_Event(Other.m_Event)
		, m_Callback(std::move(Other.m_Callback))
	{
		Other.m_Event = nullptr;
	}

	EventHandler& operator=(const EventHandler& Other)
	{
		if (this != &Other)
		{
			UnBind();

			m_Callback = Other.m_Callback;
			m_Event = Other.m_Event;

			if (m_Callback && m_Event)
			{
				m_Event->Bind(*this);
			}
			else
			{
				m_Event = nullptr;
			}
		}

		return *this;
	}

	EventHandler& operator=(EventHandler&& Other)
	{
		if (this != &Other)
		{
			UnBind();

			m_Event = Other.m_Event;
			m_Callback = std::move(Other.m_Callback);
		}

		return *this;
	}

	void Bind(EventType& Event)
	{
		assert(m_Callback && !Event);

		if (m_Callback)
		{
			m_Event = &Event;
			Event.Bind(*this);
		}
	}

	void UnBind()
	{
		if (m_Event)
		{
			m_Event->UnBind(*this);
		}
	}

	bool8_t IsBind() const
	{
		return m_Event != nullptr;
	}
private:
	friend class Event<Args...>;

	const EventType* m_Event = nullptr;
	Callback m_Callback;
};


