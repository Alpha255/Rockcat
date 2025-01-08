#ifndef __INCLUDE_MATH__
#define __INCLUDE_MATH__

#include "Shaders/Definitions.h"

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

float square(float A)
{
	return A * A;
}

float2 square(float2 A)
{
	return A * A;
}

float3 square(float3 A)
{
	return A * A;
}

float4 square(float4 A)
{
	return A * A;
}

float pow2(float A)
{
	return A * A;
}

float2 pow2(float2 A)
{
	return A * A;
}

float3 pow2(float3 A)
{
	return A * A;
}

float4 pow2(float4 A)
{
	return A * A;
}

float pow3(float A)
{
	return A * A * A;
}

float2 pow3(float2 A)
{
	return A * A * A;
}

float3 pow3(float3 A)
{
	return A * A * A;
}

float4 pow3(float4 A)
{
	return A * A * A;
}

float pow4(float A)
{
	float AA = A * A;
	return AA * AA;
}

float2 pow4(float2 A)
{
	float AA = A * A;
	return AA * AA;
}

float3 pow4(float3 A)
{
	float AA = A * A;
	return AA * AA;
}

float4 pow4(float4 A)
{
	float AA = A * A;
	return AA * AA;
}

float pow5(float A)
{
	float AA = A * A;
	return AA * AA * A;
}

float2 pow5(float2 A)
{
	float AA = A * A;
	return AA * AA * A;
}

float3 pow5(float3 A)
{
	float AA = A * A;
	return AA * AA * A;
}

float4 pow5(float4 A)
{
	float AA = A * A;
	return AA * AA * A;
}

float pow6(float A)
{
	float AA = A * A;
	return AA * AA * AA;
}

float2 pow6(float2 A)
{
	float AA = A * A;
	return AA * AA * AA;
}

float3 pow6(float3 A)
{
	float AA = A * A;
	return AA * AA * AA;
}

float4 pow6(float4 A)
{
	float AA = A * A;
	return AA * AA * AA;
}

#endif  // __INCLUDE_MATH__