#include "DepthStencilTest.h"

INT WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppDepthStencilTest depthStencilTest;

	depthStencilTest.Startup(L"DepthStencilTest");

	depthStencilTest.Running();

	depthStencilTest.ShutDown();

	return 0;
}