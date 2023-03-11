#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"
#include "Shaders/StandardPBR/Common.hlsli"

#define NUM_SAMPLES 1024

float SchlicksmithGGX(float NDotL, float NDotV, float roughness)
{
    float k = (roughness * roughness) / 2.0f;
    float GL = NDotL / (NDotL * (1.0f - k) + k);
    float GV = NDotV / (NDotV * (1.0f - k) + k);
    return GL * GV;
}

float2 BRDF(float NDotV, float roughness)
{
    float3 N = float3(0.0f, 0.0f, 1.0f);
    float3 V = float3(sqrt(1.0f - NDotV * NDotV), 0.0f, NDotV);

    float2 LUT = float2(0.0f, 0.0f)'

    for (uint i = 0; i < NUM_SAMPLES; ++i)
    {
        float2 xi = Hammersley2D(i, NUM_SAMPLES);
        float3 H = ImportanceSample_GGX(xi, roughtness, N);
        float3 L = 2.0f * dot(V, H) * H - V;

        float NDotL = max(dot(N, L), 0.0f);
        float NDotV = max(dot(N, V), 0.0f);
        float VDotH = max(dot(V, H), 0.0f);
        float NDotH = max(dot(H, N), 0.0f);

        if (NDotL > 0.0f)
        {
            float G = SchlicksmithGGX(NDotL, NDotV, roughness);
            float G_Vis = (G * VDotH) / (NDotH * NDotV);
            float Fc = pow(1.0f - VDotH, 5.0f);
            LUT += float2((1.0f - Fc) * G_Vis, Fc * G_Vis);
        }
    }

    return LUT / float(NUM_SAMPLES);
}

float4 main(float2 UV : TEXCOORD0) : SV_TARGET
{
    return float4(BRDF(UV.x, UV.y), 0.0f, 1.0f);
}