#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

TextureCube T_Environment : register(t0);
SamplerState S_LinearSampler : register(s0);

struct PushConstants
{
    float DeltaPhi;
    float DeltaTheta;
};

VK_PUSH_CONSTANT PushConstants C_PushConstants;

float4 main(float3 Pos : TEXCOORD0) : SV_TARGET
{
    float3 N = normalize(Pos);
    float3 Up = float3(0.0f, 1.0f, 0.0f);
    float3 Right = normalize(cross(Up, N));
    Up = cross(N, Right);

    float3 Color = float3(0.0f, 0.0f, 0.0f);
    uint SampleCount = 0u;
    for (float Phi = 0.0f; Phi < MATH_PI_2; Phi += C_PushConstants.DeltaPhi)
    {
        for (float Theta = 0.0f; Theta < MATH_PI_2; Theta += C_PushConstants.DeltaTheta)
        {
            float3 Vec = cos(Phi) * right + sin(Phi) * Up;
            float SampleVec = cos(Theta) * N + sin(Theta) * Vec;
            Color += T_Environment.Sample(S_LinearSampler, SampleVec).rgb * cos(Theta) * sin(Theta);
            ++SampleCount;
        }
    }

    return float4(MATH_PI * Color / float(SampleCount), 1.0f);
}