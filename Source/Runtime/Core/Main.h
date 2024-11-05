#pragma once

#include "Engine/Engine.h"

#if defined(PLATFORM_WIN32)
	#include <Windows.h>
#endif

#if defined(PLATFORM_WIN32)
int32_t WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*Commandline*/, _In_ int32_t /*ShowCmd*/)
#else
int32_t main(int32_t /*Argc*/, char** /*Argv*/)
#endif
{
	Engine::Get().Run();

	return 0;
}