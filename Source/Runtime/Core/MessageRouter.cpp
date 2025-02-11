#include "Core/MessageRouter.h"
#include "Core/Window.h"

MessageRouter& MessageRouter::Get()
{
	static MessageRouter s_Instance;
	return s_Instance;
}

void MessageRouter::RegisterMessageHandler(MessageHandler* Handler)
{
	/// #TODO: Bugs when enable multi applications
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


