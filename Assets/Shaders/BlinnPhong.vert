#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

#if 0
cbuffer c_BlingPhong_vs
{
};

void main(
    in VertexInput i_Input,
    in uint i_Instance : SV_InstanceID,
    out VertexOutput o_Output)
{
}
#else
struct VSInput
{
    VK_LOCATION(0) float3 Pos : POSITION0;
    VK_LOCATION(1) float2 UV : TEXCOORD0;
    VK_LOCATION(2) float3 Normal : NORMAL0;
};

struct UBO
{
	float4x4 projection;
	float4x4 model;
	float4 viewPos;
	float lodBias;
};

cbuffer ubo : register(b0) { UBO ubo; }

struct VSOutput
{
	float4 Pos : SV_POSITION;
    VK_LOCATION(0) float2 UV : TEXCOORD0;
    VK_LOCATION(1) float LodBias : TEXCOORD3;
    VK_LOCATION(2) float3 Normal : NORMAL0;
    VK_LOCATION(3) float3 ViewVec : TEXCOORD1;
    VK_LOCATION(4) float3 LightVec : TEXCOORD2;
};

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.UV = input.UV;
	output.LodBias = ubo.lodBias;

	float3 worldPos = mul(ubo.model, float4(input.Pos, 1.0)).xyz;

	output.Pos = mul(ubo.projection, mul(ubo.model, float4(input.Pos.xyz, 1.0)));

    float4 pos = mul(ubo.model, float4(input.Pos, 1.0));
	output.Normal = mul((float3x3)ubo.model, input.Normal);
	float3 lightPos = float3(0.0, 0.0, 0.0);
	float3 lPos = mul((float3x3)ubo.model, lightPos.xyz);
    output.LightVec = lPos - pos.xyz;
    output.ViewVec = ubo.viewPos.xyz - pos.xyz;
	return output;
}
#endif