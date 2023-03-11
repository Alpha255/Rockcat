#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"
#include "Shaders/StandardPBR/Common.hlsli"

TextureCube T_Environment : register(t0);
SamplerState S_LinearSampler : register(s0);

struct PushConstants
{
    float Roughness;
    uint NumSamples;
};
VK_PUSH_CONSTANT PushConstants C_PushConstants;

float3 PrefilterEnvironmentMap(float3 R, float Roughness)
{
    float3 N = R;
    float3 V = R;
    float3 Color = float3(0.0f, 0.0f, 0.0f);
    float TotalWeight = 0.0f;
    int2 EnvMapDims;
    T_Envionment.GetDimension(EnvMapDims.x, EnvMapDims.y);
    float EnvMapDim = float(EnvMapDims.x);
    for (uint i = 0u; i < C_PushConstants.NumSamples; ++i)
    {
        float2 Xi = Hammersley2D(i, C_PushConstants.NumSamples);
        float3 H = ImportanceSample_GGX(Xi, Roughness, N);
        float3 L = 2.0f * dot(V, H) * H - V;
        float NDotL = clamp(dot(N, L), 0.0f, 1.0f);
        if (NDotL > 0.0f)
        {
            // Filtering based on https://placeholderart.wordpress.com/2015/07/28/implementation-notes-runtime-environment-map-filtering-for-image-based-lighting/
            float NDotH = clamp(dot(N, H), 0.0f, 1.0f);
            float VDotH = clamp(dot(V, H), 0.0f, 1.0f);

            // Probability Distribution Function
            float PDF = D_GGX(NDotH, Roughness) * NDotH / (4.0f * VDotH) + 0.0001f;

            // Solid angle of current sample
            float OmegaS = 1.0f / (float(C_PushConstants.NumSamples) * PDf);

            // Solid angle of 1 pixel across all cube faces
            float OmegaP = 4.0f * MATH_PI / (6.0f * EnvMapDim * EnvMapDim);

            // Biased (+1.0) mip level for better result
            float MipLevel = Roughness == 0.0f ? 0.0f : max(0.5f * log2(OmegaS / OmegaP) + 1.0f, 0.0f);
            Color += T_Environment.SampleLevel(S_LinearSampler, L, MipLevel).rgb * NDotL;
            TotalWeight += NDotL;
        }
    }

    return Color / TotalWeight;
}

float4 main(float3 Pos : POSITION) : SV_TARGET
{
    float3 N = normalize(Pos);
    return float4(PrefilterEnvironmentMap(N, C_PushConstants.Roughness), 1.0f);
}

