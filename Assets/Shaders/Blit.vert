#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct VSInput
{
    VK_LOCATION(0) float3 Pos : POSITION;
    VK_LOCATION(1) float3 UV : TEXCOORD0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    VK_LOCATION(0) float2 UV : TEXCOORD0;
};

#if _GPU_GEN_TRIANGLE_ || _GPU_GEN_QUAD_
VSOutput main(uint VertexID : SV_VertexID)
#else
VSOutput main(VSInput input)
#endif
{
    VSOutput output;

#if _GPU_GEN_TRIANGLE_
    output.UV = float2((VertexID << 1) & 2, VertexID & 2);
    output.Pos = float4(output.UV.x * 2.0f - 1.0f, 1.0f - output.UV.y * 2.0f, 0.0f, 1.0f);
#elif _GPU_GEN_QUAD_
    uint u = VertexID & 1;
    uint v = (VertexID >> 1) & 1;

    output.Pos = float4(float(u) * 2.0f - 1.0f, 1.0f - float(v) * 2.0f, 0.0f, 1.0f);
    output.UV = float2(u, v);
#else
    output.Pos = float4(input.Pos.x, -input.Pos.y, input.Pos.z, 1.0f); /// How about DirectX?
    output.UV = input.UV.xy;    
#endif

    return output;
}