#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct VSInput
{
    VK_LOCATION(0) float3 Pos : POSITION;
};

struct PushConstants
{
    float4x4 MVP;
}

VK_PUSH_CONSTANT PushConstants C_PushConstants;

struct VSOutput
{
    float4 Pos : SV_POSITION;
    VK_LOCATION(0) float3 UVW : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.UVW = input.Pos;
    output.Pos = mul(C_PushConstants.MVP, float4(input.Pos.xyz, 1.0f));
    return output;
}