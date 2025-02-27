#include "Engine/RHI/RHIFormat.h"
#include <gl/GL.h>
#include <vulkan/vulkan_format_traits.hpp>

/// https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format

#define RHI_FORMAT_ATTRIBUTE(RHIFormat, DXGIFormat, VulkanFormat, BytesPerPixel, BitsPerPixel) RHIFormatAttributes{ ERHIFormat::RHIFormat, DXGI_FORMAT::DXGIFormat, VkFormat::VulkanFormat, 0u, 0u, 0u, BytesPerPixel, BitsPerPixel, 0u, 0u, #RHIFormat }

static std::vector<RHIFormatAttributes> s_FormatAttributes
{
	RHI_FORMAT_ATTRIBUTE(Unknown,                  DXGI_FORMAT_UNKNOWN,                    VK_FORMAT_UNDEFINED,                0u,  0u),
	RHI_FORMAT_ATTRIBUTE(D16_UNorm,                DXGI_FORMAT_D16_UNORM,                  VK_FORMAT_D16_UNORM,                2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(D32_Float,                DXGI_FORMAT_D32_FLOAT,                  VK_FORMAT_D32_SFLOAT,               4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(D32_Float_S8_UInt,        DXGI_FORMAT_D32_FLOAT_S8X24_UINT,       VK_FORMAT_D32_SFLOAT_S8_UINT,       8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(D24_UNorm_S8_UInt,        DXGI_FORMAT_D24_UNORM_S8_UINT,          VK_FORMAT_D24_UNORM_S8_UINT,        4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA32_Typeless,          DXGI_FORMAT_R32G32B32A32_TYPELESS,      VK_FORMAT_R32G32B32A32_SFLOAT,      16u, 128u),
	RHI_FORMAT_ATTRIBUTE(RGBA32_Float,             DXGI_FORMAT_R32G32B32A32_FLOAT,         VK_FORMAT_R32G32B32A32_SFLOAT,      16u, 128u),
	RHI_FORMAT_ATTRIBUTE(RGBA32_UInt,              DXGI_FORMAT_R32G32B32A32_UINT,          VK_FORMAT_R32G32B32A32_UINT,        16u, 128u),
	RHI_FORMAT_ATTRIBUTE(RGBA32_SInt,              DXGI_FORMAT_R32G32B32A32_SINT,          VK_FORMAT_R32G32B32A32_SINT,        16u, 128u),
	RHI_FORMAT_ATTRIBUTE(RGB32_Typeless,           DXGI_FORMAT_R32G32B32_TYPELESS,         VK_FORMAT_R32G32B32_SFLOAT,         12u, 96u), 
	RHI_FORMAT_ATTRIBUTE(RGB32_Float,              DXGI_FORMAT_R32G32B32_FLOAT,            VK_FORMAT_R32G32B32_SFLOAT,         12u, 96u), 
	RHI_FORMAT_ATTRIBUTE(RGB32_UInt,               DXGI_FORMAT_R32G32B32_UINT,             VK_FORMAT_R32G32B32_UINT,           12u, 96u), 
	RHI_FORMAT_ATTRIBUTE(RGB32_SInt,               DXGI_FORMAT_R32G32B32_SINT,             VK_FORMAT_R32G32B32_SINT,           12u, 96u), 
	RHI_FORMAT_ATTRIBUTE(RGBA16_Typeless,          DXGI_FORMAT_R16G16B16A16_TYPELESS,      VK_FORMAT_R16G16B16A16_SFLOAT,      8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RGBA16_Float,             DXGI_FORMAT_R16G16B16A16_FLOAT,         VK_FORMAT_R16G16B16A16_SFLOAT,      8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RGBA16_UNorm,             DXGI_FORMAT_R16G16B16A16_UNORM,         VK_FORMAT_R16G16B16A16_UNORM,       8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RGBA16_UInt,              DXGI_FORMAT_R16G16B16A16_UINT,          VK_FORMAT_R16G16B16A16_UINT,        8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RGBA16_SNorm,             DXGI_FORMAT_R16G16B16A16_SNORM,         VK_FORMAT_R16G16B16A16_SNORM,       8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RGBA16_SInt,              DXGI_FORMAT_R16G16B16A16_SINT,          VK_FORMAT_R16G16B16A16_SINT,        8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RG32_Typeless,            DXGI_FORMAT_R32G32_TYPELESS,            VK_FORMAT_R32G32_SFLOAT,            8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RG32_Float,               DXGI_FORMAT_R32G32_FLOAT,               VK_FORMAT_R32G32_SFLOAT,            8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RG32_UInt,                DXGI_FORMAT_R32G32_UINT,                VK_FORMAT_R32G32_UINT,              8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(RG32_SInt,                DXGI_FORMAT_R32G32_SINT,                VK_FORMAT_R32G32_SINT,              8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(R32G8X24_Typeless,        DXGI_FORMAT_R32G8X24_TYPELESS,          VK_FORMAT_D32_SFLOAT_S8_UINT,       8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(R32_Float_X8X24_Typeless, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,   VK_FORMAT_D32_SFLOAT_S8_UINT,       8u,  64u),
	RHI_FORMAT_ATTRIBUTE(X32_Typeless_G8X24_UInt,  DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,    VK_FORMAT_D32_SFLOAT_S8_UINT,       8u,  64u),
	RHI_FORMAT_ATTRIBUTE(RGB10A2_Typeless,         DXGI_FORMAT_R10G10B10A2_TYPELESS,       VK_FORMAT_A2B10G10R10_UNORM_PACK32, 4u,  32u),
	RHI_FORMAT_ATTRIBUTE(RGB10A2_UNorm,            DXGI_FORMAT_R10G10B10A2_UNORM,          VK_FORMAT_A2B10G10R10_UNORM_PACK32, 4u,  32u),
	RHI_FORMAT_ATTRIBUTE(RGB10A2_UInt,             DXGI_FORMAT_R10G10B10A2_UINT,           VK_FORMAT_A2B10G10R10_UNORM_PACK32, 4u,  32u),
	RHI_FORMAT_ATTRIBUTE(RG11B10_Float,            DXGI_FORMAT_R11G11B10_FLOAT,            VK_FORMAT_B10G11R11_UFLOAT_PACK32,  4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA8_Typeless,           DXGI_FORMAT_R8G8B8A8_TYPELESS,          VK_FORMAT_R8G8B8A8_UNORM,           4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA8_UNorm,              DXGI_FORMAT_R8G8B8A8_UNORM,             VK_FORMAT_R8G8B8A8_UNORM,           4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA8_UNorm_SRGB,         DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,        VK_FORMAT_R8G8B8A8_SRGB,            4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA8_UInt,               DXGI_FORMAT_R8G8B8A8_UINT,              VK_FORMAT_R8G8B8A8_UINT,            4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA8_SNorm,              DXGI_FORMAT_R8G8B8A8_SNORM,             VK_FORMAT_R8G8B8A8_SNORM,           4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGBA8_SInt,               DXGI_FORMAT_R8G8B8A8_SINT,              VK_FORMAT_R8G8B8A8_SINT,            4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG16_Typeless,            DXGI_FORMAT_R16G16_TYPELESS,            VK_FORMAT_R16G16_SFLOAT,            4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG16_Float,               DXGI_FORMAT_R16G16_FLOAT,               VK_FORMAT_R16G16_SFLOAT,            4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG16_UNorm,               DXGI_FORMAT_R16G16_UNORM,               VK_FORMAT_R16G16_UNORM,             4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG16_UInt,                DXGI_FORMAT_R16G16_UINT,                VK_FORMAT_R16G16_UINT,              4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG16_SNorm,               DXGI_FORMAT_R16G16_SNORM,               VK_FORMAT_R16G16_SNORM,             4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG16_SInt,                DXGI_FORMAT_R16G16_SINT,                VK_FORMAT_R16G16_SINT,              4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(R32_Typeless,             DXGI_FORMAT_R32_TYPELESS,               VK_FORMAT_R32_SFLOAT,               4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(R32_Float,                DXGI_FORMAT_R32_FLOAT,                  VK_FORMAT_R32_SFLOAT,               4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(R32_UInt,                 DXGI_FORMAT_R32_UINT,                   VK_FORMAT_R32_UINT,                 4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(R32_SInt,                 DXGI_FORMAT_R32_SINT,                   VK_FORMAT_R32_SINT,                 4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(R24G8_Typeless,           DXGI_FORMAT_R24G8_TYPELESS,             VK_FORMAT_D24_UNORM_S8_UINT,        4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(R24_UNorm_X8_Typeless,    DXGI_FORMAT_R24_UNORM_X8_TYPELESS,      VK_FORMAT_D24_UNORM_S8_UINT,        4u,  32u),
	RHI_FORMAT_ATTRIBUTE(X24_Typelesss_G8_UInt,    DXGI_FORMAT_X24_TYPELESS_G8_UINT,       VK_FORMAT_D24_UNORM_S8_UINT,        4u,  32u),
	RHI_FORMAT_ATTRIBUTE(RG8_Typeless,             DXGI_FORMAT_R8G8_TYPELESS,              VK_FORMAT_R8G8_UNORM,               2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(RG8_UNorm,                DXGI_FORMAT_R8G8_UNORM,                 VK_FORMAT_R8G8_UNORM,               2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(RG8_UInt,                 DXGI_FORMAT_R8G8_UINT,                  VK_FORMAT_R8G8_UINT,                2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(RG8_SNorm,                DXGI_FORMAT_R8G8_SNORM,                 VK_FORMAT_R8G8_SNORM,               2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(RG8_SInt,                 DXGI_FORMAT_R8G8_SINT,                  VK_FORMAT_R8G8_SINT,                2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R16_Typeless,             DXGI_FORMAT_R16_TYPELESS,               VK_FORMAT_R16_SFLOAT,               2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R16_Float,                DXGI_FORMAT_R16_FLOAT,                  VK_FORMAT_R16_SFLOAT,               2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R16_UNorm,                DXGI_FORMAT_R16_UNORM,                  VK_FORMAT_R16_UNORM,                2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R16_UInt,                 DXGI_FORMAT_R16_UINT,                   VK_FORMAT_R16_UINT,                 2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R16_SNorm,                DXGI_FORMAT_R16_SNORM,                  VK_FORMAT_R16_SNORM,                2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R16_SInt,                 DXGI_FORMAT_R16_SINT,                   VK_FORMAT_R16_SINT,                 2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(R8_Typeless,              DXGI_FORMAT_R8_TYPELESS,                VK_FORMAT_R8_UNORM,                 1u,  8u),  
	RHI_FORMAT_ATTRIBUTE(R8_UNorm,                 DXGI_FORMAT_R8_UNORM,                   VK_FORMAT_R8_UNORM,                 1u,  8u),  
	RHI_FORMAT_ATTRIBUTE(R8_UInt,                  DXGI_FORMAT_R8_UINT,                    VK_FORMAT_R8_UINT,                  1u,  8u),  
	RHI_FORMAT_ATTRIBUTE(R8_SNorm,                 DXGI_FORMAT_R8_SNORM,                   VK_FORMAT_R8_SNORM,                 1u,  8u),  
	RHI_FORMAT_ATTRIBUTE(R8_SInt,                  DXGI_FORMAT_R8_SINT,                    VK_FORMAT_R8_SINT,                  1u,  8u),  
	RHI_FORMAT_ATTRIBUTE(A8_UNorm,                 DXGI_FORMAT_A8_UNORM,                   VK_FORMAT_UNDEFINED,                1u,  8u),  
	RHI_FORMAT_ATTRIBUTE(R1_UNorm,                 DXGI_FORMAT_R1_UNORM,                   VK_FORMAT_UNDEFINED,                0u,  1u),  
	RHI_FORMAT_ATTRIBUTE(RGB9E5_SharedXP,          DXGI_FORMAT_R9G9B9E5_SHAREDEXP,         VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RG8_BG8_UNorm,            DXGI_FORMAT_R8G8_B8G8_UNORM,            VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(GR8_GB8_UNorm,            DXGI_FORMAT_G8R8_G8B8_UNORM,            VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(BC1_Typeless,             DXGI_FORMAT_BC1_TYPELESS,               VK_FORMAT_BC1_RGB_UNORM_BLOCK,      8u,  4u),  
	RHI_FORMAT_ATTRIBUTE(BC1_UNorm,                DXGI_FORMAT_BC1_UNORM,                  VK_FORMAT_BC1_RGB_UNORM_BLOCK,      8u,  4u),  
	RHI_FORMAT_ATTRIBUTE(BC1_UNorm_SRGB,           DXGI_FORMAT_BC1_UNORM_SRGB,             VK_FORMAT_BC1_RGB_SRGB_BLOCK,       8u,  4u),  
	RHI_FORMAT_ATTRIBUTE(BC2_Typeless,             DXGI_FORMAT_BC2_TYPELESS,               VK_FORMAT_BC2_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC2_UNorm,                DXGI_FORMAT_BC2_UNORM,                  VK_FORMAT_BC2_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC2_UNorm_SRGB,           DXGI_FORMAT_BC2_UNORM_SRGB,             VK_FORMAT_BC2_SRGB_BLOCK,           16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC3_Typeless,             DXGI_FORMAT_BC3_TYPELESS,               VK_FORMAT_BC3_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC3_UNorm,                DXGI_FORMAT_BC3_UNORM,                  VK_FORMAT_BC3_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC3_UNorm_SRGB,           DXGI_FORMAT_BC3_UNORM_SRGB,             VK_FORMAT_BC3_SRGB_BLOCK,           16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC4_Typeless,             DXGI_FORMAT_BC4_TYPELESS,               VK_FORMAT_BC4_UNORM_BLOCK,          8u,  4u),  
	RHI_FORMAT_ATTRIBUTE(BC4_UNorm,                DXGI_FORMAT_BC4_UNORM,                  VK_FORMAT_BC4_UNORM_BLOCK,          8u,  4u),  
	RHI_FORMAT_ATTRIBUTE(BC4_SNorm,                DXGI_FORMAT_BC4_SNORM,                  VK_FORMAT_BC4_UNORM_BLOCK,          8u,  4u),  
	RHI_FORMAT_ATTRIBUTE(BC5_Typeless,             DXGI_FORMAT_BC5_TYPELESS,               VK_FORMAT_BC5_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC5_UNorm,                DXGI_FORMAT_BC5_UNORM,                  VK_FORMAT_BC5_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC5_SNorm,                DXGI_FORMAT_BC5_SNORM,                  VK_FORMAT_BC5_SNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(B5G6R5_UNorm,             DXGI_FORMAT_B5G6R5_UNORM,               VK_FORMAT_B5G6R5_UNORM_PACK16,      2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(BGR5A1_UNorm,             DXGI_FORMAT_B5G5R5A1_UNORM,             VK_FORMAT_B5G5R5A1_UNORM_PACK16,    2u,  16u), 
	RHI_FORMAT_ATTRIBUTE(BGRA8_Typeless,           DXGI_FORMAT_B8G8R8A8_TYPELESS,          VK_FORMAT_B8G8R8A8_UNORM,           4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(BGRA8_UNorm,              DXGI_FORMAT_B8G8R8A8_UNORM,             VK_FORMAT_B8G8R8A8_UNORM,           4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(BGRA8_UNorm_SRGB,         DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,        VK_FORMAT_B8G8R8A8_SRGB,            4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(BGRX8_Typeless,           DXGI_FORMAT_B8G8R8X8_TYPELESS,          VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(BGRX8_UNorm,              DXGI_FORMAT_B8G8R8X8_UNORM,             VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(BGRX8_UNorm_SRGB,         DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,        VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(RGB10_XR_Bias_A2_UNorm,   DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, VK_FORMAT_UNDEFINED,                4u,  32u),
	RHI_FORMAT_ATTRIBUTE(BC6H_Typeless,            DXGI_FORMAT_BC6H_TYPELESS,              VK_FORMAT_BC6H_UFLOAT_BLOCK,        16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC6H_UF16,                DXGI_FORMAT_BC6H_UF16,                  VK_FORMAT_BC6H_UFLOAT_BLOCK,        16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC6H_SF16,                DXGI_FORMAT_BC6H_SF16,                  VK_FORMAT_BC6H_SFLOAT_BLOCK,        16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC7_Typeless,             DXGI_FORMAT_BC7_TYPELESS,               VK_FORMAT_BC7_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC7_UNorm,                DXGI_FORMAT_BC7_UNORM,                  VK_FORMAT_BC7_UNORM_BLOCK,          16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(BC7_UNorm_SRGB,           DXGI_FORMAT_BC7_UNORM_SRGB,             VK_FORMAT_BC7_SRGB_BLOCK,           16u, 8u),  
	RHI_FORMAT_ATTRIBUTE(AYUV,                     DXGI_FORMAT_AYUV,                       VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(Y410,                     DXGI_FORMAT_Y410,                       VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(Y416,                     DXGI_FORMAT_Y416,                       VK_FORMAT_UNDEFINED,                8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(NV12,                     DXGI_FORMAT_NV12,                       VK_FORMAT_UNDEFINED,                2u,  12u), 
	RHI_FORMAT_ATTRIBUTE(P010,                     DXGI_FORMAT_P010,                       VK_FORMAT_UNDEFINED,                4u,  24u), 
	RHI_FORMAT_ATTRIBUTE(P016,                     DXGI_FORMAT_P016,                       VK_FORMAT_UNDEFINED,                4u,  24u), 
	RHI_FORMAT_ATTRIBUTE(Opaque_420,               DXGI_FORMAT_420_OPAQUE,                 VK_FORMAT_UNDEFINED,                2u,  12u), 
	RHI_FORMAT_ATTRIBUTE(YUY2,                     DXGI_FORMAT_YUY2,                       VK_FORMAT_UNDEFINED,                4u,  32u), 
	RHI_FORMAT_ATTRIBUTE(Y210,                     DXGI_FORMAT_Y210,                       VK_FORMAT_UNDEFINED,                8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(Y216,                     DXGI_FORMAT_Y216,                       VK_FORMAT_UNDEFINED,                8u,  64u), 
	RHI_FORMAT_ATTRIBUTE(NV11,                     DXGI_FORMAT_NV11,                       VK_FORMAT_UNDEFINED,                0u,  12u)
};

namespace RHI
{
	ERHIFormat GetRHIFormatByName(const char* Name)
	{
		for (auto& Attrs : s_FormatAttributes)
		{
			if (Attrs.Name == Name)
			{
				return Attrs.RHIFormat;
			}
		}
		return ERHIFormat::Unknown;
	}

	DXGI_FORMAT GetDXGIFormat(ERHIFormat Format)
	{
		assert(s_FormatAttributes[static_cast<uint32_t>(Format)].RHIFormat == Format);
		return s_FormatAttributes[static_cast<uint32_t>(Format)].DXGIFormat;
	}

	vk::Format GetVulkanFormat(ERHIFormat Format)
	{
		assert(s_FormatAttributes[static_cast<uint32_t>(Format)].RHIFormat == Format);
		return static_cast<vk::Format>(s_FormatAttributes[static_cast<uint32_t>(Format)].VulkanFormat);
	}

	ERHIFormat GetRHIFormat(vk::Format vkFormat)
	{
		for (auto& Attrs : s_FormatAttributes)
		{
			if (static_cast<vk::Format>(Attrs.VulkanFormat) == vkFormat)
			{
				return Attrs.RHIFormat;
			}
		}
		return ERHIFormat::Unknown;
	}

	ERHIFormat GetRHIFormat(DXGI_FORMAT DXGIFormat)
	{
		for (auto& Attrs : s_FormatAttributes)
		{
			if (Attrs.DXGIFormat == DXGIFormat)
			{
				return Attrs.RHIFormat;
			}
		}
		return ERHIFormat::Unknown;
	}

	ERHIFormat GetSRGBFormat(ERHIFormat Format)
	{
		switch (Format)
		{
		case ERHIFormat::BC1_UNorm:   return ERHIFormat::BC1_UNorm_SRGB;
		case ERHIFormat::BC2_UNorm:   return ERHIFormat::BC2_UNorm_SRGB;
		case ERHIFormat::BGRA8_UNorm: return ERHIFormat::BGRA8_UNorm_SRGB;
		case ERHIFormat::BGRX8_UNorm: return ERHIFormat::BGRX8_UNorm_SRGB;
		case ERHIFormat::BC7_UNorm:   return ERHIFormat::BC7_UNorm_SRGB;
		case ERHIFormat::RGBA8_UNorm: return ERHIFormat::RGBA8_UNorm_SRGB;
		default: return Format;
		}
	}

	bool IsColor(ERHIFormat Format)
	{
		return !(IsDepth(Format) || IsDepthStencil(Format)) && Format != ERHIFormat::Unknown;
	}

	bool IsDepth(ERHIFormat Format)
	{
		return Format == ERHIFormat::D16_UNorm || Format == ERHIFormat::D32_Float;
	}

	bool IsDepthStencil(ERHIFormat Format)
	{
		return Format == ERHIFormat::D24_UNorm_S8_UInt || Format == ERHIFormat::D32_Float_S8_UInt;
	}

	RHIFormatAttributes GetFormatAttributes(uint32_t Width, uint32_t Height, ERHIFormat Format)
	{
		bool IsCompressed =
			Format == ERHIFormat::BC1_Typeless ||
			Format == ERHIFormat::BC1_UNorm ||
			Format == ERHIFormat::BC1_UNorm_SRGB ||
			Format == ERHIFormat::BC2_Typeless ||
			Format == ERHIFormat::BC2_UNorm ||
			Format == ERHIFormat::BC2_UNorm_SRGB ||
			Format == ERHIFormat::BC3_Typeless ||
			Format == ERHIFormat::BC3_UNorm ||
			Format == ERHIFormat::BC3_UNorm_SRGB ||
			Format == ERHIFormat::BC4_Typeless ||
			Format == ERHIFormat::BC4_UNorm ||
			Format == ERHIFormat::BC4_SNorm ||
			Format == ERHIFormat::BC5_Typeless ||
			Format == ERHIFormat::BC5_UNorm ||
			Format == ERHIFormat::BC5_SNorm ||
			Format == ERHIFormat::BC6H_Typeless ||
			Format == ERHIFormat::BC6H_UF16 ||
			Format == ERHIFormat::BC6H_SF16 ||
			Format == ERHIFormat::BC7_Typeless ||
			Format == ERHIFormat::BC7_UNorm ||
			Format == ERHIFormat::BC7_UNorm_SRGB;

		bool IsPlanar =
			Format == ERHIFormat::NV12 ||
			Format == ERHIFormat::Opaque_420 ||
			Format == ERHIFormat::P010 ||
			Format == ERHIFormat::P016;

		bool IsPacked =
			Format == ERHIFormat::RG8_BG8_UNorm ||
			Format == ERHIFormat::GR8_GB8_UNorm ||
			Format == ERHIFormat::YUY2 ||
			Format == ERHIFormat::Y210 ||
			Format == ERHIFormat::Y216;

		RHIFormatAttributes& Attributes = s_FormatAttributes[static_cast<size_t>(Format)];
		Attributes.NumCols = Width;
		Attributes.NumRows = Height;
		Attributes.BlockSize = IsCompressed ? 4u : 1u;

		if (IsCompressed)
		{
			uint32_t BlockWidth = 0u;
			if (Width > 0u)
			{
				BlockWidth = std::max<uint32_t>(1u, (Width + 3u) / 4u);
			}
			uint32_t BlockHeight = 0u;
			if (Height > 0u)
			{
				BlockHeight = std::max<uint32_t>(1u, (Height + 3u) / 4u);
			}
			Attributes.RowPitch = BlockWidth * Attributes.BytesPerPixel;
			Attributes.SlicePitch = Attributes.RowPitch * BlockHeight;
			Attributes.NumRows = BlockWidth;
			Attributes.NumCols = BlockHeight;
		}
		else if (IsPacked)
		{
			Attributes.RowPitch = ((Width + 1u) >> 1u) * Attributes.BytesPerPixel;
			Attributes.SlicePitch = Attributes.RowPitch * Height;
		}
		else if (Format == ERHIFormat::NV11)
		{
			Attributes.RowPitch = ((Width + 3u) >> 2u) * 4u;
			Attributes.NumRows = Height * 2u;
			Attributes.SlicePitch = Attributes.RowPitch * Attributes.NumRows;
		}
		else if (IsPlanar)
		{
			Attributes.RowPitch = ((Width + 1u) >> 1u) * Attributes.BytesPerPixel;
			Attributes.SlicePitch = (Attributes.RowPitch * Height) + ((Attributes.RowPitch * Height + 1u) >> 1u);
			Attributes.NumRows = Height + ((Height + 1u) >> 1u);
		}
		else
		{
			assert(Attributes.BitsPerPixel);
			Attributes.RowPitch = (Width * Attributes.BitsPerPixel + 7u) / 8u;
			Attributes.SlicePitch = Attributes.RowPitch * Height;
		}

		return Attributes;
	}
}
