#include "DisplacementMap.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppDisplacement DisplacementMap;

	DisplacementMap.Startup(L"DisplacementMap");

	DisplacementMap.Running();

	DisplacementMap.ShutDown();

	return 0;
}