#include "EventRouter.h"
#include "Window.h"

EventHandler::EventHandler(EEventMask Mask)
	: m_MessageMask(Mask)
{
	EventRouter::Get().RegisterEventHandler(this);
}

void EventRouter::RegisterEventHandler(EventHandler* Handler)
{
	if (Handler && std::find(m_Handlers.cbegin(), m_Handlers.cend(), Handler) == m_Handlers.cend())
	{
		m_Handlers.push_back(Handler);
	}
}

void EventRouter::DeregisterEventHandler(EventHandler* Handler)
{
	if (Handler)
	{
		auto It = std::find(m_Handlers.cbegin(), m_Handlers.cend(), Handler);
		if (It != m_Handlers.end())
		{
			m_Handlers.erase(It);
		}
	}
}


