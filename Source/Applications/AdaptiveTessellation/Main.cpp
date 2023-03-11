#include "AdaptiveTessellation.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppAdaptiveTessellation AdaptiveTessellation;

	AdaptiveTessellation.Startup(L"DemoAdaptiveTessellation");

	AdaptiveTessellation.Running();

	AdaptiveTessellation.ShutDown();

	return 0;
}