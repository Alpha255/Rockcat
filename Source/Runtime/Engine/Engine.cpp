#include "Runtime/Engine/Engine.h"

Engine& Engine::Get()
{
	static Engine s_Engine;
	return s_Engine;
}

void Engine::Run()
{
	if (!m_Initialized)
	{
		m_Initialized = Initialize();
		assert(m_Initialized);
	}
}

void Engine::RunApplication(BaseApplication& Application)
{
}

bool8_t Engine::Initialize()
{
	return false;
}

void Engine::Finalize()
{
}
