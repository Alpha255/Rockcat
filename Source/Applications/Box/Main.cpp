#include "Box.h"

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	Box box;

	box.Initialize("Box");

	box.Loop();

	box.Finalize();

	return 0;
}