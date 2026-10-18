#include "Application/Application.h"

#if PLATFORM_WINDOWS
#include <Windows.h>

int32_t WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*Commandline*/, _In_ int32_t /*ShowCmd*/)
{
	return 0;
}
#else
	#error Unknown platform
#endif