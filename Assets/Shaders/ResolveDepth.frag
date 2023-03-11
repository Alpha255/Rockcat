#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

Texture2DMS<float4> UnresolvedDepth;

void main(float2 UV : TEXCOORD0, out float OutDepth : SV_DEPTH)
{
#ifndef RESOLVE_NUM_SAMPLES
    float3 Dimensions;
    UnresolvedDepth.GetDimensions(Dimensions.x, Dimensions.y, Dimensions.z);
    int NumSamples = (int)Dimensions.z;
#else
    int NumSamples = RESOLVE_NUM_SAMPLES;
#endif

    int2 IntUV = trunc(UV);
    float2 ResolvedDepth = UnresolvedDepth.Load(IntUV, 0).r;

    for (int SampleIndex = 1; SampleIndex < NumSamples; ++SampleIndex)
    {
        float2 Sample = UnresolvedDepth.Load(IntUV, SampleIndex).r;

        ResolvedDepth = max(ResolvedDepth, Sample);
    }

    OutDepth = ResolvedDepth;
}