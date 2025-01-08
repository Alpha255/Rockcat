#ifndef __INCLUDE_BRDF__
#define __INCLUDE_BRDF__

#include "Shaders/Algorithm.hlsli"

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX(float Roughness, float NDotH)
{
	float Alpha2 = pow4(Roughness);
	float Denom = (NDotH * Alpha2 - NDotH) * NDotH + 1.0f;
	return Alpha2 / (MATH_PI * Denom * Denom);
}

/// Fresnel
float3 Fresnel_Schlick(float F0, float HDotV)
{
	return F0 + (1.0f - F0) * pow5(1.0f - HDotV);
}

// Tuned to match behavior of Vis_Smith
// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float Geometry_SchlickSmithGGX(float NDotL, float NDotV, float Roughness)
{
	float R = Roughness + 1.0f;
	float K = (R * R) * 0.125f;
	float GL = NDotL / (NDotL * (1.0f - K) + K);
	float GV = NDotV / (NDotV * (1.0f - K) + K);
	return GL * GV;
}

#endif
