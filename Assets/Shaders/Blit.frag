#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

VK_BINDING(0, 0) Texture2D ColorImage : register(t0);
VK_BINDING(1, 0) SamplerState LinearSampler : register(s0);

struct VSOutput
{
    VK_LOCATION(0) float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
    return ColorImage.Sample(LinearSampler, input.UV);
}