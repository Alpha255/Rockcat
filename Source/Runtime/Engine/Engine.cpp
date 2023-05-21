#include "Runtime/Engine/Engine.h"

Engine& Engine::Get()
{
	static Engine s_Engine;
	return s_Engine;
}
