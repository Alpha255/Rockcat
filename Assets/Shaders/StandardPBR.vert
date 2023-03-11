#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct VSInput
{
    VK_LOCATION(0) float3 Pos : POSITION;
    VK_LOCATION(1) float3 Normal : NORMAL;
    VK_LOCATION(2) float3 Tangent : TANGENT;
    VK_LOCATION(3) float3 Bitangent : BITANGENT;
    VK_LOCATION(4) float3 UV : TEXCOORD0;
};

struct UBO
{
    float4x4 Projection;
    float4x4 Model;
    float4x4 View;
    float3 CameraPos;
};

VK_BINDING(0, 0) cbuffer ubo : register(b0)
{
    UBO ubo;
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
    VK_OFFSET(0) float3 ObjectPos;
};
VK_PUSH_CONSTANT PushConstants pushConstant;

VSOutput main(VSInput input)
{
    VSOutput output;
    float3 localPos = mul(float4(input.Pos, 1.0), ubo.Model).xyz;
    output.WorldPos = localPos + pushConstant.ObjectPos;
    output.Normal = mul(input.Normal, (float3x3)ubo.Model);
    output.Pos = mul(mul(float4(output.WorldPos, 1.0), ubo.View), ubo.Projection);
    output.UV = input.UV.xy;
    return output;
}