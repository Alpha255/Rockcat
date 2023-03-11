#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct VSInput
{
    VK_LOCATION(0) float3 Pos : POSITION;
    VK_LOCATION(1) float3 Normal : NORMAL;
    VK_LOCATION(2) float2 UV : TEXCOORD0;
};

cbuffer
{
    float4x4 C_Projection;
    float4x4 C_World;
    float4x4 C_View;
    float3 C_CameraPos;
}

struct VSOutput
{
    float4 Pos : SV_POSITION;
    VK_LOCATION(0) float3 WorldPos : POSITION;
    VK_LOCATION(1) float3 Normal : NORMAL;
    VK_LOCATION(2) float2 UV : TEXCOORD0;
};

struct PushConstants
{
    float Delta;
};

VK_PUSH_CONSTANT PushConstants C_PushConstants;

VSOutput main(VSInput Input)
{
    VSOutput Output;
    float3 LocalPos = mul(C_World, float4(Input.Pos, 1.0f)).xyz;
    Output.WorldPos = LocalPos + C_PushConstants.Delta;
    Output.Normal = mul((float3x3)C_World, Input.Normal);
    Output.UV = Input.UV;
    Output.UV.y = 1.0f - Input.UV.y;
    Output.Pos = mul(C_Projection, mul(C_View, float4(Output.WorldPos, 1.0f)));
    return Output;
}