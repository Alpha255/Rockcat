#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct VSOutput
{
	VK_LOCATION(0) float3 WorldPos : POSITION;
	VK_LOCATION(1) float3 Normal : NORMAL;
	VK_LOCATION(2) float2 UV : TEXCOORD0;
};

VK_BINDING(1, 0) cbuffer UBO : register(b0)
{
	float4x4 Projection;
	float4x4 Model;
	float4x4 View;
	float3 CameraPos;

	float4 LightPos;
}

struct PushConstans
{
	float Roughness;
	float Metallic;
};

VK_PUSH_CONSTANT PushConstans MaterialParams;

VK_BINDING(2, 0) Texture2D AlbedoImage : register(t1);
VK_BINDING(3, 0) SamplerState LinearSampler : register(s1);

/// Normal Distrbution
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return alpha2 / (MATH_PI * denom * denom);
}

/// Geometric Shadowing
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

/// Fresnel
float3 F_Schlick(float cosTheta, float metallic, float3 albedo)
{
	float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
	float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	return F;
}

float3 BRDF(float3 L, float3 V, float3 N, float metallic, float roughness, float3 albedo)
{
	float3 H = normalize(V + L);
	float dotNV = saturate(dot(N, V));
	float dotNL = saturate(dot(N, L));
	float dotLH = saturate(dot(L, H));
	float dotNH = saturate(dot(N, H));

	float3 lightColor = float3(1.0, 1.0, 1.0);

	float3 color = float3(0.0, 0.0, 0.0);

	[branch]
	if (dotNL > 0.0)
	{
		float D = D_GGX(dotNH, roughness);
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		float3 F = F_Schlick(dotNV, metallic, albedo);

		float3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * lightColor;
	}

	return color;
}

float4 main(VSOutput input) : SV_TARGET
{
	float3 N = normalize(input.Normal);
	float3 V = normalize(CameraPos - input.WorldPos);

	float roughness = MaterialParams.Roughness;

	float3 albedo = AlbedoImage.Sample(LinearSampler, input.UV).rgb;

	float3 L = normalize(LightPos.xyz - input.WorldPos);
	float3 Lo = BRDF(L, V, N, MaterialParams.Metallic, roughness, albedo);

	float3 color = albedo * 0.02 + Lo;

	color = pow(color, float3(0.4545, 0.4545, 0.4545));

	return float4(color, 1.0);
}