#include "OceanFFT.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppOceanFFT app;

	app.Startup(L"OceanFFT");

	app.Running();

	app.ShutDown();

	return 0;
}