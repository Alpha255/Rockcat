#include "Shaders/Definitions.h"
#include "Shaders/Algorithm.hlsli"

DECLARE_GLOBAL_BLINN_PHONG_FS_VARIABLES

float4 main(VSOutput Input) : SV_TARGET
{
	float4 Diffuse = DiffuseMap_Sampler.SampleLevel(DiffuseMap, Input.UV0);

#ifdef _HAS_NORMALMAP_
	float3 Normal = NormalMap_Sampler.SampleLevel(NormalMap, Input.UV0).rgb;
	Normal = ComputeWorldNormal(Input.WorldNormal, Normal);
#else
	Normal = normalize(Input.WorldNormal);
#endif

#if 0
	float3 LightDirection = normalize(ViewPosition - LightPosition);
	float3 ViewDirection = normalize(ViewPosition - Input.WorldPosition);
	float3 Reflect = reflect(-LightDirection, Normal);
	
	float3 diffuse = max(dot(N, L), 0.0) * float3(1.0, 1.0, 1.0);
	float specular = pow(max(dot(R, V), 0.0), 16.0) * color.a;

	return float4(diffuse * color.rgb + specular, 1.0);
#endif
	return Diffuse;
}