#ifndef _INCLUDE_VERTEX_INPUT_LAYOUT_
#define _INCLUDE_VERTEX_INPUT_LAYOUT_

struct VertexInput
{
    float3 Position : POSITION;
#if HAS_NORMAL
    float3 Normal : NORMAL;
#endif
#if HAS_TANGENT
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
#endif
#if HAS_UV0
    float2 UV0 : TEXCOORD0;
#endif
#if HAS_UV1
    float2 UV1 : TEXCOORD;
#endif
#if HAS_COLOR
    float4 Color : COLOR;
#endif
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float3 PositionW : POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR;
};

#endif