#include "Shaders/Definitions.h"

DEFINITION_SHADER_VARIABLES_DEPTH_ONLY

void main(VSOutput Input)
{
#if _HAS_UV0_
	if (SAMPLE_TEXTURE2D(BaseColorMap, Input.UV0).a < AlphaCutoff)
	{
		discard;
	}
#endif
}