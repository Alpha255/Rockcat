#pragma once

#include "Engine/Engine.h"

#if defined(PLATFORM_WIN32)
	#include <Windows.h>
#endif

#if defined(PLATFORM_WIN32)
int32_t WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*Commandline*/, int32_t /*ShowCmd*/)
#else
int32_t main(int32_t /*Argc*/, char** /*Argv*/)
#endif
{
	Engine::Get().Run();

	return 0;
}