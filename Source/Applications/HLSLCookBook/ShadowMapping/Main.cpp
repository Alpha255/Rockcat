#include "ShadowMapping.h"

int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)
{
	ShadowMapping shadowMapping;

	shadowMapping.Initialize("ShadowMapping");

	shadowMapping.Loop();

	shadowMapping.Finalize();

	return 0;
}