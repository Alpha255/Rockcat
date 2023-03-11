#include "Shaders/Definitions.hlsli"
#include "Shaders/VertexInputLayout.hlsli"

#define DOWNSAMPLE_DEPTH_FILTER_POINT    0
#define DOWNSAMPLE_DEPTH_FILTER_MAX      1
#define DOWNSAMPLE_DEPTH_FILTER_CBMINMAX 2

Texture2D<float> SceneDepth;
float4 TexelOffsetsHorizontal; // float4(0, 0, 1 / Width, 0)
float4 TexelOffsetsVertical;   // float4(0, 1 / Height, 1 / Width, 1 / Height)
float4 BufferSizeAndInvSize;
float2 MaxUV;
float2 DestinationResolution;
uint DownsampleDepthFilter;

float GetDepth(float2 UV)
{
    return SceneDepth.Sample(GlobalPointClampedSampler, min(UV, MaxUV));
}

void main(
    noperspective float4 UV : TEXCOORD0,
    out float OutDepth : SV_DEPTH,
    out float4 OutColor : SV_TARGET0)
{
    OutColor = 0.0f;
    float Depth = 0.0f;

    if (DownsampleDepthFilter < DOWNSAMPLE_DEPTH_FILTER_CBMINMAX)
    {
        // Lookup the four DeviceZ's of the full resolution pixels corresponding to this low resolution pixel
        float Depth0 = GetDepth(UV.xy + TexelOffsetHorizontal.xy);
        float Depth1 = GetDepth(UV.xy + TexelOffsetHorizontal.zw);
        float Depth2 = GetDepth(UV.xy + TexelOffsetVertical.xy);
        float Depth3 = GetDepth(UV.xy + TexelOffsetVertical.zw);

#if INVERSED_DEPTH
        float FarDepth = min(min(Depth0, Depth1), min(Depth2, Depth3));
#else
        float FarDepth = max(max(Depth0, Depth1), max(Depth2, Depth3));
#endif
		// Max depth shrinks the silhouettes around foreground objects and is conservative for depth testing
		// Sample 0 has consistent error, use whichever one is requested for this downsample
        Depth = DownsampleDepthFilter == DOWNSAMPLE_DEPTH_FILTER_MAX ? FarDepth : Depth0;
    }
    else  // DOWNSAMPLE_DEPTH_FILTER_CBMINMAX
    {
        const float OffsetScale = 0.25f;
        // OffsetScale is 0.25 because:
		//  - UV is centered on the low resolution render targetr in UV space
		//  - SourceTexelOffsets23 in fact represent a texel size for the DESTINATION render target (TODO refactor that code once in main)
		//  - So scale by 0.5 will get us to the edge of the half resolution texel. 
		//  - And to sample the center of the high resolution source pixel in UV space we need to divide angain the offset by half ==> 0.25
        float Depth0 = GetDepth(UV.xy + TexelOffsetVertical.zw * OffsetScale * float2(-1.0f, -1.0f));
        float Depth1 = GetDepth(UV.xy + TexelOffsetVertical.zw * OffsetScale * float2(-1.0f, +1.0f));
        float Depth2 = GetDepth(UV.xy + TexelOffsetVertical.zw * OffsetScale * float2(+1.0f, -1.0f));
        float Depth3 = GetDepth(UV.xy + TexelOffsetVertical.zw * OffsetScale * float2(+1.0f, +1.0f));

        const float MaxDepth = max(max(Depth0, Depth1), max(Depth2, Depth3));
        const float MinDepth = min(min(Depth0, Depth1), min(Depth2, Depth3));

        const float2 Resolution = uint2(DestinationResolution * BufferSizeAndInvSize.xy * BufferSizeAndInvSize.zw);
        const uint2 PixelPos = uint2(UV.xy * Resolution);

        const uint2 PixelPosStep = (PixelPos >> 1) * 2;
        uint CheckerBoard = PixelPos.x - PixelPosStep.x;                                        // horizontal alternance of black and white
        CheckerBoard = (PixelPos.y - PixelPosStep.y) == 0 ? CheckerBoard : 1.0 - CheckerBoard;  // vertical toggle of horizontal checker on odd lines
        Depth = CheckerBoard > 0.0f ? MaxDepth : MinDepth;
    }

    OutDepth = Depth;
}