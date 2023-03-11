#include "Lighting.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppLighting lighting;

	lighting.Startup(L"Lighting");

	lighting.Running();

	lighting.ShutDown();

	return 0;
}