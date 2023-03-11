#pragma once

#include "Colorful/IRenderer/Declarations.h"

NAMESPACE_START(RHI)

enum class EFormat : uint32_t
{
	Unknown,
	D16_UNorm,
	D32_Float,
	D32_Float_S8_UInt,
	D24_UNorm_S8_UInt,
	RGBA32_Typeless,
	RGBA32_Float,
	RGBA32_UInt,
	RGBA32_SInt,
	RGB32_Typeless,
	RGB32_Float,
	RGB32_UInt,
	RGB32_SInt,
	RGBA16_Typeless,
	RGBA16_Float,
	RGBA16_UNorm,
	RGBA16_UInt,
	RGBA16_SNorm,
	RGBA16_SInt,
	RG32_Typeless,
	RG32_Float,
	RG32_UInt,
	RG32_SInt,
	R32G8X24_Typeless,
	R32_Float_X8X24_Typeless,
	X32_Typeless_G8X24_UInt,
	RGB10A2_Typeless,
	RGB10A2_UNorm,
	RGB10A2_UInt,
	RG11B10_Float,
	RGBA8_Typeless,
	RGBA8_UNorm,
	RGBA8_UNorm_SRGB,
	RGBA8_UInt,
	RGBA8_SNorm,
	RGBA8_SInt,
	RG16_Typeless,
	RG16_Float,
	RG16_UNorm,
	RG16_UInt,
	RG16_SNorm,
	RG16_SInt,
	R32_Typeless,
	R32_Float,
	R32_UInt,
	R32_SInt,
	R24G8_Typeless,
	R24_UNorm_X8_Typeless,
	X24_Typelesss_G8_UInt,
	RG8_Typeless,
	RG8_UNorm,
	RG8_UInt,
	RG8_SNorm,
	RG8_SInt,
	R16_Typeless,
	R16_Float,
	R16_UNorm,
	R16_UInt,
	R16_SNorm,
	R16_SInt,
	R8_Typeless,
	R8_UNorm,
	R8_UInt,
	R8_SNorm,
	R8_SInt,
	A8_UNorm,
	R1_UNorm,
	RGB9E5_SharedXP,
	RG8_BG8_UNorm,
	GR8_GB8_UNorm,
	BC1_Typeless,
	BC1_UNorm,
	BC1_UNorm_SRGB,
	BC2_Typeless,
	BC2_UNorm,
	BC2_UNorm_SRGB,
	BC3_Typeless,
	BC3_UNorm,
	BC3_UNorm_SRGB,
	BC4_Typeless,
	BC4_UNorm,
	BC4_SNorm,
	BC5_Typeless,
	BC5_UNorm,
	BC5_SNorm,
	B5G6R5_UNorm,
	BGR5A1_UNorm,
	BGRA8_Typeless,
	BGRA8_UNorm,
	BGRA8_UNorm_SRGB,
	BGRX8_Typeless,
	BGRX8_UNorm,
	BGRX8_UNorm_SRGB,
	RGB10_XR_Bias_A2_UNorm,
	BC6H_Typeless,
	BC6H_UF16,
	BC6H_SF16,
	BC7_Typeless,
	BC7_UNorm,
	BC7_UNorm_SRGB,
	AYUV,
	Y410,
	Y416,
	NV12,
	P010,
	P016,
	Opaque_420,
	YUY2,
	Y210,
	Y216,
	NV11
};

struct FormatAttribute
{
	EFormat Format = EFormat::Unknown;
	int32_t DXGIFromat = 0u;
	uint32_t VkFormat = 0u;
	uint32_t GLFormat = 0u;
	uint32_t BytesPerPixel = 0u;
	uint32_t BitsPerPixel = 0u;
	const char8_t* Name = nullptr;

	static FormatAttribute Attribute(EFormat Format);
	static FormatAttribute Attribute(const char8_t* const Format);
	static FormatAttribute Attribute_Vk(uint32_t Format);
	static FormatAttribute Attribute_DXGI(uint32_t Format);
	static FormatAttribute Attribute_GL(uint32_t Format);
	static uint32_t ToDXGIFormat(EFormat Format);
	static uint32_t ToVulkanFormat(EFormat Format);
	static EFormat ToSRGBFormat(EFormat Format);

	static bool8_t IsColor(EFormat Format);
	static bool8_t IsDepth(EFormat Format);
	static bool8_t IsDepthStenci(EFormat Format);

	static std::pair<uint32_t, uint32_t> CalculateFormatBytes(
		uint32_t Width,
		uint32_t Height,
		EFormat Format,
		__out uint32_t& SliceBytes,
		__out uint32_t& RowBytes);
};

NAMESPACE_END(RHI)
