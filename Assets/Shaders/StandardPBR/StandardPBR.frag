#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"
#include "Shaders/StandardPBR/Common.hlsli"

struct VSOutput
{
    VK_LOCATION(0) float3 WorldPos : POSITION;
    VK_LOCATION(1) float3 Normal : NORMAL;
    VK_LOCATION(2) float2 UV : TEXCOORD0;
};

cbuffer
{
    float4x4 C_Projection;
    float4x4 C_World;
    float4x4 C_View;
    float3 C_CameraPos;
    float C_Exposure;
    float4 C_LightDirs[4];
    float C_Gamma;
}

struct PushConstants
{
    float Roughness;
    float Metallic;
    float Specular;
    float3 Albedo;
};

VK_PUSH_CONSTANT PushConstants C_PushConstants;

TextureCube T_IrradianceCube;
Texture2D T_BRDFLUT;
TextureCube T_PrefilteredCube;
SamplerState S_LinearSampler;

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float NDotL, float NDotV, float Roughness)
{
    float R = Roughness + 1.0f;
    float K = (R * R) / 8.0f;
    float GL = NDotL / (NDotL * (1.0f - K) + K);
    float GV = NDotV / (NDotV * (1.0f - K) + K);
    return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float CosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - CosTheta, 5.0f);
}

float3 F_SchlickR(float CosTheta, float3 F0, float Roughness)
{
    return F0 + (max((1.0f - Roughness).xxx, F0) - F0) * pow(1.0f - CosTheta, 5.0f);
}

float3 PrefilteredReflection(float3 R, float Roughness)
{
    const float MAX_REFLECTION_LOD = 9.0f;
    float LOD = Roughness * MAX_REFLECTION_LOD;
    float LODf = floor(LOD);
    float LODc = ceil(LOD);
    float3 A = T_PrefilteredCube.SampleLevel(S_LinearSampler, R, LODf).rgb;
    float3 B = T_PrefilteredCube.SampleLevel(S_LinearSampler, R, LODc).rgb;
    return lerp(A, B, LOD - LODf);
}

float3 SpecularTerm(float3 L, float3 V, float3 N, float3 F0, float Metallic, float Roughness)
{
    float3 H = normalize(V + L);
    float NDotH = clamp(dot(N, H), 0.0f, 1.0f);
    float NDotV = clamp(dot(N, V), 0.0f, 1.0f);
    float NDotL = clamp(dot(N, L), 0.0f, 1.0f);

    /// Fixed light color
    float3 LightColor = float3(1.0f, 1.0f, 1.0f);
    float3 Color = float3(0.0f, 0.0f, 0.0f);

    if (NDotL > 0.0f)
    {
        float D = D_GGX(NDotH, Roughness);
        float G = G_SchlicksmithGGX(NDotL, NDotV, Roughness);
        float3 F = F_Schlick(NDotV, F0);
        float3 Specular = D * F * G / (4.0f * NDotL * NDotV + 0.001f);
        float3 KD = (float3(1.0f, 1.0f, 1.0f) - F) * (1.0f - Metallic);
        Color += (KD * C_PushConstants.Albedo / MATH_PI + Specular) * NDotL;
    }

    return Color;
}

float4 main(VSOutput Input) : SV_TARGET
{
    float3 N = normalize(Input.Normal);
    float3 V = normalize(C_CameraPos - Input.WorldPos);
    float3 R = reflect(-V, N);

    float Metallic = C_PushConstants.Metallic;
    float Roughness = C_PushConstants.Roughness;

    float3 F0 = float3(0.0f, 0.0f, 0.0f);
    F0 = lerp(F0, C_PushConstants.Albedo, Metallic);

    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        float3 L = normalize(C_LightDirs[i].xyz - Input.WorldPos);
        Lo += SpecularTerm(L, V, N, F0, Metallic, Roughness);
    }

    float2 BRDF = T_BRDFLUT.Sample(S_LinearSampler, float2(max(dot(N, V), 0.0f), Roughness)).rg;
    float3 Reflection = PrefilteredReflection(R, Roughness).rgb;
    float3 Irradiance = T_IrradianceCube.Sample(S_LinearSampler, N).rgb;

    // Diffuse based on irradiance
    float3 Diffuse = Irradiance * C_PushConstants.Albedo;
    float3 F = F_SchlickR(max(dot(N, V), 0.0f), F0, Roughness);

    // Specular reflectance
    float3 Specular = Reflection * (F * BRDF.x + BRDF.y);

    // Ambient term
    float3 KD = 1.0f - F;
    KD *= 1.0f - Metallic;
    float3 Ambient = (KD * Diffuse + Specular);

    float3 Color = Ambient + Lo;

    // Tone mapping
    Color  = Tonemap_Uncharted2(Color * C_Exposure);
    Color = Color * (1.0f / Tonemap_Uncharted2((11.2f).xxx));

    // Gamma correction
    Color = pow(Color, (1.0f / C_Gamma).xxx);

    return float4(Color, 1.0f);
}
