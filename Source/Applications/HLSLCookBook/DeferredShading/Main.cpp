#include "DeferredShading.h"

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	DeferredShading deferredShading;

	deferredShading.Initialize("DeferredShading");

	deferredShading.Loop();

	deferredShading.Finalize();

	return 0;
}
