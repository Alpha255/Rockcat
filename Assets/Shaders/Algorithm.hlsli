#ifndef __INCLUDE_ALGORITHM__
#define __INCLUDE_ALGORITHM__

float min3(float A, float B, float C)
{
    return min(A, min(B, C));
}

float2 min3(float2 A, float2 B, float2 C)
{
    return float2(
        min3(A.x, B.x, C.x),
        min3(A.y, B.y, C.y)
    );
}

float3 min3(float3 A, float3 B, float3 C)
{
    return float3(
        min3(A.x, B.x, C.x),
        min3(A.y, B.y, C.y),
        min3(A.z, B.z, C.z)
    );
}

float4 min3(float4 A, float4 B, float4 C)
{
    return float4(
        min3(A.x, B.x, C.x),
        min3(A.y, B.y, C.y),
        min3(A.z, B.z, C.z),
        min3(A.w, B.w, C.w)
    );
}

float max3(float A, float B, float C)
{
    return max(A, max(B, C));
}

float2 max3(float2 A, float2 B, float2 C)
{
    return float2(
        max3(A.x, B.x, C.x),
        max3(A.y, B.y, C.y)
    );
}

float3 max3(float3 A, float3 B, float3 C)
{
    return float3(
        max3(A.x, B.x, C.x),
        max3(A.y, B.y, C.y),
        max3(A.z, B.z, C.z)
    );
}

float4 max3(float4 A, float4 B, float4 C)
{
    return float4(
        max3(A.x, B.x, C.x),
        max3(A.y, B.y, C.y),
        max3(A.z, B.z, C.z),
        max3(A.w, B.w, C.w)
    );
}

half min3(half A, half B, half C)
{
    return min(A, min(B, C));
}

half2 min3(half2 A, half2 B, half2 C)
{
    return half2(
        min3(A.x, B.x, C.x),
        min3(A.y, B.y, C.y)
    );
}

half3 min3(half3 A, half3 B, half3 C)
{
    return half3(
        min3(A.x, B.x, C.x),
        min3(A.y, B.y, C.y),
        min3(A.z, B.z, C.z)
    );
}

half4 min3(half4 A, half4 B, half4 C)
{
    return half4(
        min3(A.x, B.x, C.x),
        min3(A.y, B.y, C.y),
        min3(A.z, B.z, C.z),
        min3(A.w, B.w, C.w)
    );
}

half max3(half A, half B, half C)
{
    return max(A, max(B, C));
}

half2 max3(half2 A, half2 B, half2 C)
{
    return half2(
        max3(A.x, B.x, C.x),
        max3(A.y, B.y, C.y)
    );
}

half3 max3(half3 A, half3 B, half3 C)
{
    return half3(
        max3(A.x, B.x, C.x),
        max3(A.y, B.y, C.y),
        max3(A.z, B.z, C.z)
    );
}

half4 max3(half4 A, half4 B, half4 C)
{
    return half4(
        max3(A.x, B.x, C.x),
        max3(A.y, B.y, C.y),
        max3(A.z, B.z, C.z),
        max3(A.w, B.w, C.w)
    );
}

float3 ComputeWorldNormal(VSOutput Output, float3 NormalFromTexture)
{
    float3 WorldNormal = normalize(Output.WorldNormal);

#if _HAS_TANGENT
    float3 WorldTangent = normalize(Output.WorldTangent);
    float3 Bitangent = normalize(cross(WorldNormal, WorldTangent));
    float3x3 TBN = float3x3(WorldTangent, Bitangent, WorldNormal);

    WorldNormal = NormalFromTexture * 2.0 - 1.0;
    WorldNormal = mul(WorldNormal, TBN); // Transpose
#endif
    return WorldNormal;
}

#endif  // __INCLUDE_ALGORITHM__