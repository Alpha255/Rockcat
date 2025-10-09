#include "Shaders/Definitions.h"

DEFINE_SHADER_VARIABLES_DEPTH_ONLY_FS

void main(VSOutput Input)
{
#if _HAS_UV0_
	if (SAMPLE_TEXTURE2D(BaseColorMap, Input.UV0).a < MaterialProperty.AlphaCutoff)
	{
		discard;
	}
#endif
}