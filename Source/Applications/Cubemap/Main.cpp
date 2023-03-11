#include "Cubemap.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppCubemap cubemap;

	cubemap.Startup(L"Cubemap");

	cubemap.Running();

	cubemap.ShutDown();

	return 0;
}