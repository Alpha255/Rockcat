#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

struct VSOutput
{
    float4 Pos : SV_POSITION;
    VK_LOCATION(0) float2 UV : TEXCOORD0;
};

VSOutput main(uint VertexIndex : SV_VERTEXID)
{
    VSOutput Output;
    Output.UV = float2((VertexID << 1) & 2, VertexID & 2);
    Output.Pos = float4(Output.UV.x * 2.0f - 1.0f, 1.0f - Output.UV.y * 2.0f, 0.0f, 1.0f);
    return Output;
}

