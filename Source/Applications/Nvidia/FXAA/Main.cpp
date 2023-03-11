#include "FXAA.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	ApplicationFXAA FXAA;

	FXAA.Startup(L"FXAA");

	FXAA.Running();

	FXAA.ShutDown();

	return 0;
}