#include "Core/MessageRouter.h"
#include "Core/Window.h"

void MessageRouter::RegisterMessageHandler(MessageHandler* Handler)
{
	if (Handler && std::find(m_Handlers.cbegin(), m_Handlers.cend(), Handler) == m_Handlers.cend())
	{
		m_Handlers.push_back(Handler);
	}
}

void MessageRouter::DeregisterMessageHandler(MessageHandler* Handler)
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


