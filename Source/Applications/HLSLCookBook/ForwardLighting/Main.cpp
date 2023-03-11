#include "ForwardLighting.h"

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	ForwardLighting forwardLighting;

	forwardLighting.Initialize("ForwardLighting");

	forwardLighting.Loop();

	forwardLighting.Finalize();

	return 0;
}