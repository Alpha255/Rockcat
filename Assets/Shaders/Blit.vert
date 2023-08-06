#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

#if _GPU_GEN_TRIANGLE_ || _GPU_GEN_QUAD_
VertexOutput main(uint VertexID : SV_VertexID)
#else
VertexOutput main(VertexInput Input)
#endif
{
	VertexOutput Output;

#if _HAS_UV0_

#if _GPU_GEN_TRIANGLE_
    Output.UV0 = float2((VertexID << 1) & 2, VertexID & 2);
    Output.Pos = float4(output.UV0.x * 2.0f - 1.0f, 1.0f - output.UV0.y * 2.0f, 0.0f, 1.0f);
#elif _GPU_GEN_QUAD_
    uint U = VertexID & 1;
    uint V = (VertexID >> 1) & 1;

    output.Pos = float4(float(U) * 2.0f - 1.0f, 1.0f - float(V) * 2.0f, 0.0f, 1.0f);
    output.UV0 = float2(U, V);
#else
    output.Pos = float4(input.Pos.x, -input.Pos.y, input.Pos.z, 1.0f); /// How about DirectX?
    output.UV0 = input.UV0.xy;    
#endif

#endif  // _HAS_UV0_

    return output;
}