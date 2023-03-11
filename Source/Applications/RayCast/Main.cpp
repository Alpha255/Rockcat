
#include "RayCast.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppRayCast RayCast;

	RayCast.Startup(L"RayCast");

	RayCast.Running();

	RayCast.ShutDown();

	return 0;
}
