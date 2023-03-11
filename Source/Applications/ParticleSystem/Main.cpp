#include "AppParticleSystem.h"

int32_t WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nShow*/)
{
	AppParticleSystem app;

	app.Startup(L"ParticleSystem");

	app.Running();

	app.ShutDown();

	return 0;
}