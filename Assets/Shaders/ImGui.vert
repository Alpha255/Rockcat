#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct PushConstants
{
    VK_OFFSET(0) float2 InverseDisplaySize;
};

struct VSInput
{
    VK_LOCATION(0) float2 Pos : POSITION;
    VK_LOCATION(1) float2 UV : TEXCOORD0;
    VK_LOCATION(2) float4 Color : COLOR0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    VK_LOCATION(0) float4 Color : COLOR0;
    VK_LOCATION(1) float2 UV  : TEXCOORD0;
};

VK_PUSH_CONSTANT PushConstants Constants;

VSOutput main(VSInput input)
{
    VSOutput output;
    output.Pos.xy = input.Pos.xy * Constants.InverseDisplaySize * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    output.Pos.zw = float2(0.0f, 1.0f);
    output.Color = input.Color;
    output.UV = input.UV;
    return output;
}

