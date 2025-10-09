#include "Shaders/Definitions.h"

DEFINE_SHADER_VARIABLES_GENERIC_VS

VSOutput main(VSInput Input)
{	
	VSOutput Output = (VSOutput)0;

	float4 WorldPosition = mul(WVP.WorldMatrix, float4(Input.Position, 1.0));

	Output.WorldPosition = WorldPosition.xyz;
	Output.Position = mul(WVP.ProjectionMatrix, mul(WVP.ViewMatrix, WorldPosition));

#if _HAS_NORMAL_
	Output.WorldNormal = mul(WVP.WorldMatrix, float4(Input.Normal, 1.0)).xyz;
#endif

#if _HAS_TANGENT_
	Output.WorldTangent = mul(WVP.WorldMatrix, float4(Input.Tangent, 1.0)).xyz;
	Output.WorldBiTangent = mul(WVP.WorldMatrix, float4(Input.BiTangent, 1.0)).xyz;
#endif

#if _HAS_UV0_
	Output.UV0 = Input.UV0;
#endif

#if _HAS_UV1_
	Output.UV1 = Input.UV1;
#endif

#if _HAS_COLOR_
	Output.Color = Input.Color;
#endif

	return Output;
}