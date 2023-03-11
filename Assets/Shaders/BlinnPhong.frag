#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

Texture2D textureColor : register(t1);
SamplerState samplerColor : register(s1);

struct VSOutput
{
	VK_LOCATION(0) float2 UV : TEXCOORD0;
	VK_LOCATION(1) float LodBias : TEXCOORD3;
	VK_LOCATION(2) float3 Normal : NORMAL0;
	VK_LOCATION(3) float3 ViewVec : TEXCOORD1;
	VK_LOCATION(4) float3 LightVec : TEXCOORD2;
};

float4 main(VSOutput input) : SV_TARGET
{
	float4 color = textureColor.SampleLevel(samplerColor, input.UV, input.LodBias);

	float3 N = normalize(input.Normal);
	float3 L = normalize(input.LightVec);
	float3 V = normalize(input.ViewVec);
	float3 R = reflect(-L, N);
	float3 diffuse = max(dot(N, L), 0.0) * float3(1.0, 1.0, 1.0);
	float specular = pow(max(dot(R, V), 0.0), 16.0) * color.a;

	return float4(diffuse * color.rgb + specular, 1.0);
}