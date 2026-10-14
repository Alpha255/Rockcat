#include "Shaders/Definitions.hlsli"

struct PushConstants
{
    VK_OFFSET(0) float2 InverseDisplaySize;
};

struct VSInput
{
    VK_LOCATION(0) float2 Pos : POSITION;
    VK_LOCATION(1) float2 UV : TEXCOORD0;
    VK_LOCATION(2) float4 Color : COLOR0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    VK_LOCATION(0) float4 Color : COLOR0;
    VK_LOCATION(1) float2 UV  : TEXCOORD0;
};

VK_PUSH_CONSTANT PushConstants Constants;

VSOutput main(VSInput input)
{
/// ImGui hands us positions in pixels with y growing downwards from the top of the screen.
/// The two APIs disagree about where +1 in clip space lands, so the y axis has to be mirrored
/// for one of them, otherwise everything is drawn upside down.
#ifdef _SPIRV_
    /// Vulkan puts the framebuffer origin at the top left, so y = -1 is the top of the screen.
    const float2 ClipScale = float2(2.0f, 2.0f);
    const float2 ClipBias = float2(-1.0f, -1.0f);
#else
    /// D3D has +1 at the top, which is exactly the opposite.
    const float2 ClipScale = float2(2.0f, -2.0f);
    const float2 ClipBias = float2(-1.0f, 1.0f);
#endif

    VSOutput output;
    output.Pos.xy = input.Pos.xy * Constants.InverseDisplaySize * ClipScale + ClipBias;
    output.Pos.zw = float2(0.0f, 1.0f);
    output.Color = input.Color;
    output.UV = input.UV;
    return output;
}

