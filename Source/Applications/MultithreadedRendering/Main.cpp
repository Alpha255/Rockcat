
#include "MultithreadedRendering.h"

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	MultithreadedRendering multithreadedRendering;

	multithreadedRendering.Initialize("MultithreadedRendering");

	multithreadedRendering.Loop();

	multithreadedRendering.Finalize();

	return 0;
}
