#include "Shaders/Definitions.h"

DECLARE_GLOBAL_GENERIC_VS_VARIABLES

cbuffer CBuffer : register(b0)
{ 
	UniformBuffer CBuffer; 
}

VSOutput main(VsInput Input)
{
	VSOutput Output = (VSOutput)0;

	float4 WorldPosition = mul(CBuffer.WorldMatrix, float4(Input.Position, 1.0));

	Output.WorldPosition = WorldPosition.xyz;
	Output.Position = mul(CBuffer.ProjectionMatrix, mul(CBuffer.ViewMatrix, WorldPosition));
	Output.WorldNormal = mul(CBuffer.WorldMatrix, float4(Input.Normal, 1.0)).xyz;
	Output.UV0 = Input.UV0;

#if _HAS_TANGENT_
	Output.WorldTangent = mul(CBuffer.WorldMatrix, float4(Input.Tangent, 1.0)).xyz;
	Output.WorldBiTangent = mul(CBuffer.WorldMatrix, float4(Input.BiTangent, 1.0)).xyz;
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