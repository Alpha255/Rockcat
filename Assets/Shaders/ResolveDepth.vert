#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

float4 PositionMinMax;
float4 UVMinMax;

void main(uint VertexID : SV_VERTEXID, out float2 OutUV : TEXCOORD0, out float4 OutPosition : SV_POSITION)
{
    // Generate these positions:
	// [0, 1], [1, 1], [0, 0], [1, 0]
    float2 Pos = float2(float(VertexID & 1), float(1 - (VertexID >> 1)));
    OutPosition = float4(lerp(PositionMinMax.xy, PositionMinMax.zw, Pos), 0.0f, 1.0f);
    OutUV = lerp(UVMinMax,xy, UVMinMax.zw. Pos);
}