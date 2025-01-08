#ifndef __INCLUDE_ALGORITHM__
#define __INCLUDE_ALGORITHM__

#include "Shaders/Math.hlsli"

float3 ComputeWorldNormal(VSOutput Output, float3 NormalMapNormal)
{
	float3 WorldNormal = normalize(Output.WorldNormal);

#if _HAS_TANGENT_
	float3 WorldTangent = normalize(Output.WorldTangent);
	float3 WorldBitangent = normalize(cross(WorldNormal, WorldTangent));
	float3x3 TBN = float3x3(WorldTangent, WorldBitangent, WorldNormal);

	WorldNormal = NormalMapNormal * 2.0 - 1.0;
	WorldNormal = mul(WorldNormal, TBN); // Transpose
#endif
	return WorldNormal;
}

#endif