#include "Shaders/Algorithm.hlsli"

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

float4 main(VSOutput Input) : SV_TARGET
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