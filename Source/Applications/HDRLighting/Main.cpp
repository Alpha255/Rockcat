#include "HDRLighting.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppHDRLighting HDRLighting;

	HDRLighting.Startup(L"HDRLighting");

	HDRLighting.Running();

	HDRLighting.ShutDown();

	return 0;
}