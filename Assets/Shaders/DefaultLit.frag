#include "Shaders/Definitions.h"
#include "Shaders/Algorithm.hlsli"

DECLARE_GLOBAL_DEFAULT_LIT_FS_VARIABLES

float4 main(VSOutput Input) : SV_TARGET
{
#if _SHADING_MODE_STANDARD_PBR_
#else
#endif
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}