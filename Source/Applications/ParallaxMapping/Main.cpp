#include "ParallaxMapping.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppParallaxMapping parallaxMapping;

	parallaxMapping.Startup(L"ParallaxMapping");

	parallaxMapping.Running();

	parallaxMapping.ShutDown();

	return 0;
}