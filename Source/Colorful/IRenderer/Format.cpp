#include "Colorful/IRenderer/Format.h"
#include <dxgiformat.h>
#include <ThirdParty/Vulkan-Headers/include/vulkan/vulkan.h>
#include <ThirdParty/KTX-Software/lib/gl_format.h>

/// https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format

NAMESPACE_START(RHI)

#define GFX_FORMAT_ATTRIBUTE(Format, DXGIFormat, VulkanFormat, GLFormat, BytesPerPixel, BitsPerPixel) { EFormat::Format, DXGI_FORMAT::DXGIFormat, VkFormat::VulkanFormat, GLFormat, BytesPerPixel, BitsPerPixel, #Format }

static const std::vector<FormatAttribute> FormatAttributes
{
	GFX_FORMAT_ATTRIBUTE(Unknown,                  DXGI_FORMAT_UNKNOWN,                    VK_FORMAT_UNDEFINED,                0u,                                     0u,  0u),
	GFX_FORMAT_ATTRIBUTE(D16_UNorm,                DXGI_FORMAT_D16_UNORM,                  VK_FORMAT_D16_UNORM,                GL_DEPTH_COMPONENT16,                   2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(D32_Float,                DXGI_FORMAT_D32_FLOAT,                  VK_FORMAT_D32_SFLOAT,               GL_DEPTH_COMPONENT32F,                  4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(D32_Float_S8_UInt,        DXGI_FORMAT_D32_FLOAT_S8X24_UINT,       VK_FORMAT_D32_SFLOAT_S8_UINT,       GL_DEPTH32F_STENCIL8,                   8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(D24_UNorm_S8_UInt,        DXGI_FORMAT_D24_UNORM_S8_UINT,          VK_FORMAT_D24_UNORM_S8_UINT,        GL_DEPTH24_STENCIL8,                    4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA32_Typeless,          DXGI_FORMAT_R32G32B32A32_TYPELESS,      VK_FORMAT_R32G32B32A32_SFLOAT,      GL_RGBA32F,                             16u, 128u),
	GFX_FORMAT_ATTRIBUTE(RGBA32_Float,             DXGI_FORMAT_R32G32B32A32_FLOAT,         VK_FORMAT_R32G32B32A32_SFLOAT,      GL_RGBA32F,                             16u, 128u),
	GFX_FORMAT_ATTRIBUTE(RGBA32_UInt,              DXGI_FORMAT_R32G32B32A32_UINT,          VK_FORMAT_R32G32B32A32_UINT,        GL_RGBA32UI,                            16u, 128u),
	GFX_FORMAT_ATTRIBUTE(RGBA32_SInt,              DXGI_FORMAT_R32G32B32A32_SINT,          VK_FORMAT_R32G32B32A32_SINT,        GL_RGBA32I,                             16u, 128u),
	GFX_FORMAT_ATTRIBUTE(RGB32_Typeless,           DXGI_FORMAT_R32G32B32_TYPELESS,         VK_FORMAT_R32G32B32_SFLOAT,         GL_RGB32F,                              12u, 96u), 
	GFX_FORMAT_ATTRIBUTE(RGB32_Float,              DXGI_FORMAT_R32G32B32_FLOAT,            VK_FORMAT_R32G32B32_SFLOAT,         GL_RGB32F,                              12u, 96u), 
	GFX_FORMAT_ATTRIBUTE(RGB32_UInt,               DXGI_FORMAT_R32G32B32_UINT,             VK_FORMAT_R32G32B32_UINT,           GL_RGB32UI,                             12u, 96u), 
	GFX_FORMAT_ATTRIBUTE(RGB32_SInt,               DXGI_FORMAT_R32G32B32_SINT,             VK_FORMAT_R32G32B32_SINT,           GL_RGB32I,                              12u, 96u), 
	GFX_FORMAT_ATTRIBUTE(RGBA16_Typeless,          DXGI_FORMAT_R16G16B16A16_TYPELESS,      VK_FORMAT_R16G16B16A16_SFLOAT,      GL_RGBA16F,                             8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RGBA16_Float,             DXGI_FORMAT_R16G16B16A16_FLOAT,         VK_FORMAT_R16G16B16A16_SFLOAT,      GL_RGBA16F,                             8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RGBA16_UNorm,             DXGI_FORMAT_R16G16B16A16_UNORM,         VK_FORMAT_R16G16B16A16_UNORM,       GL_RGBA16,                              8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RGBA16_UInt,              DXGI_FORMAT_R16G16B16A16_UINT,          VK_FORMAT_R16G16B16A16_UINT,        GL_RGBA16UI,                            8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RGBA16_SNorm,             DXGI_FORMAT_R16G16B16A16_SNORM,         VK_FORMAT_R16G16B16A16_SNORM,       GL_RGBA16_SNORM,                        8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RGBA16_SInt,              DXGI_FORMAT_R16G16B16A16_SINT,          VK_FORMAT_R16G16B16A16_SINT,        GL_RGBA16I,                             8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RG32_Typeless,            DXGI_FORMAT_R32G32_TYPELESS,            VK_FORMAT_R32G32_SFLOAT,            GL_RG32F,                               8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RG32_Float,               DXGI_FORMAT_R32G32_FLOAT,               VK_FORMAT_R32G32_SFLOAT,            GL_RG32F,                               8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RG32_UInt,                DXGI_FORMAT_R32G32_UINT,                VK_FORMAT_R32G32_UINT,              GL_RG32UI,                              8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(RG32_SInt,                DXGI_FORMAT_R32G32_SINT,                VK_FORMAT_R32G32_SINT,              GL_RG32I,                               8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(R32G8X24_Typeless,        DXGI_FORMAT_R32G8X24_TYPELESS,          VK_FORMAT_D32_SFLOAT_S8_UINT,       GL_DEPTH32F_STENCIL8,                   8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(R32_Float_X8X24_Typeless, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,   VK_FORMAT_D32_SFLOAT_S8_UINT,       GL_DEPTH32F_STENCIL8,                   8u,  64u),
	GFX_FORMAT_ATTRIBUTE(X32_Typeless_G8X24_UInt,  DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,    VK_FORMAT_D32_SFLOAT_S8_UINT,       0u,                                     8u,  64u),
	GFX_FORMAT_ATTRIBUTE(RGB10A2_Typeless,         DXGI_FORMAT_R10G10B10A2_TYPELESS,       VK_FORMAT_A2B10G10R10_UNORM_PACK32, 0u,                                     4u,  32u),
	GFX_FORMAT_ATTRIBUTE(RGB10A2_UNorm,            DXGI_FORMAT_R10G10B10A2_UNORM,          VK_FORMAT_A2B10G10R10_UNORM_PACK32, 0u,                                     4u,  32u),
	GFX_FORMAT_ATTRIBUTE(RGB10A2_UInt,             DXGI_FORMAT_R10G10B10A2_UINT,           VK_FORMAT_A2B10G10R10_UNORM_PACK32, 0u,                                     4u,  32u),
	GFX_FORMAT_ATTRIBUTE(RG11B10_Float,            DXGI_FORMAT_R11G11B10_FLOAT,            VK_FORMAT_B10G11R11_UFLOAT_PACK32,  GL_R11F_G11F_B10F,                      4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA8_Typeless,           DXGI_FORMAT_R8G8B8A8_TYPELESS,          VK_FORMAT_R8G8B8A8_UNORM,           GL_RGBA8,                               4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA8_UNorm,              DXGI_FORMAT_R8G8B8A8_UNORM,             VK_FORMAT_R8G8B8A8_UNORM,           GL_RGBA8,                               4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA8_UNorm_SRGB,         DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,        VK_FORMAT_R8G8B8A8_SRGB,            GL_SRGB8_ALPHA8,                        4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA8_UInt,               DXGI_FORMAT_R8G8B8A8_UINT,              VK_FORMAT_R8G8B8A8_UINT,            GL_RGBA8UI,                             4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA8_SNorm,              DXGI_FORMAT_R8G8B8A8_SNORM,             VK_FORMAT_R8G8B8A8_SNORM,           GL_RGBA8_SNORM,                         4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGBA8_SInt,               DXGI_FORMAT_R8G8B8A8_SINT,              VK_FORMAT_R8G8B8A8_SINT,            GL_RGBA8I,                              4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG16_Typeless,            DXGI_FORMAT_R16G16_TYPELESS,            VK_FORMAT_R16G16_SFLOAT,            GL_RG16F,                               4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG16_Float,               DXGI_FORMAT_R16G16_FLOAT,               VK_FORMAT_R16G16_SFLOAT,            GL_RG16F,                               4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG16_UNorm,               DXGI_FORMAT_R16G16_UNORM,               VK_FORMAT_R16G16_UNORM,             GL_RG16,                                4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG16_UInt,                DXGI_FORMAT_R16G16_UINT,                VK_FORMAT_R16G16_UINT,              GL_RG16UI,                              4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG16_SNorm,               DXGI_FORMAT_R16G16_SNORM,               VK_FORMAT_R16G16_SNORM,             GL_RG16_SNORM,                          4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG16_SInt,                DXGI_FORMAT_R16G16_SINT,                VK_FORMAT_R16G16_SINT,              GL_RG16I,                               4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(R32_Typeless,             DXGI_FORMAT_R32_TYPELESS,               VK_FORMAT_R32_SFLOAT,               GL_R32F,                                4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(R32_Float,                DXGI_FORMAT_R32_FLOAT,                  VK_FORMAT_R32_SFLOAT,               GL_R32F,                                4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(R32_UInt,                 DXGI_FORMAT_R32_UINT,                   VK_FORMAT_R32_UINT,                 GL_R32UI,                               4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(R32_SInt,                 DXGI_FORMAT_R32_SINT,                   VK_FORMAT_R32_SINT,                 GL_R32I,                                4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(R24G8_Typeless,           DXGI_FORMAT_R24G8_TYPELESS,             VK_FORMAT_D24_UNORM_S8_UINT,        GL_DEPTH24_STENCIL8,                    4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(R24_UNorm_X8_Typeless,    DXGI_FORMAT_R24_UNORM_X8_TYPELESS,      VK_FORMAT_D24_UNORM_S8_UINT,        GL_DEPTH24_STENCIL8,                    4u,  32u),
	GFX_FORMAT_ATTRIBUTE(X24_Typelesss_G8_UInt,    DXGI_FORMAT_X24_TYPELESS_G8_UINT,       VK_FORMAT_D24_UNORM_S8_UINT,        0u,                                     4u,  32u),
	GFX_FORMAT_ATTRIBUTE(RG8_Typeless,             DXGI_FORMAT_R8G8_TYPELESS,              VK_FORMAT_R8G8_UNORM,               GL_RG8,                                 2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(RG8_UNorm,                DXGI_FORMAT_R8G8_UNORM,                 VK_FORMAT_R8G8_UNORM,               GL_RG8,                                 2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(RG8_UInt,                 DXGI_FORMAT_R8G8_UINT,                  VK_FORMAT_R8G8_UINT,                GL_RG8UI,                               2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(RG8_SNorm,                DXGI_FORMAT_R8G8_SNORM,                 VK_FORMAT_R8G8_SNORM,               GL_RG8_SNORM,                           2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(RG8_SInt,                 DXGI_FORMAT_R8G8_SINT,                  VK_FORMAT_R8G8_SINT,                GL_RG8I,                                2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R16_Typeless,             DXGI_FORMAT_R16_TYPELESS,               VK_FORMAT_R16_SFLOAT,               GL_R16F,                                2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R16_Float,                DXGI_FORMAT_R16_FLOAT,                  VK_FORMAT_R16_SFLOAT,               GL_R16F,                                2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R16_UNorm,                DXGI_FORMAT_R16_UNORM,                  VK_FORMAT_R16_UNORM,                GL_R16,                                 2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R16_UInt,                 DXGI_FORMAT_R16_UINT,                   VK_FORMAT_R16_UINT,                 GL_R16UI,                               2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R16_SNorm,                DXGI_FORMAT_R16_SNORM,                  VK_FORMAT_R16_SNORM,                GL_R16_SNORM,                           2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R16_SInt,                 DXGI_FORMAT_R16_SINT,                   VK_FORMAT_R16_SINT,                 GL_R16I,                                2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(R8_Typeless,              DXGI_FORMAT_R8_TYPELESS,                VK_FORMAT_R8_UNORM,                 GL_R8,                                  1u,  8u),  
	GFX_FORMAT_ATTRIBUTE(R8_UNorm,                 DXGI_FORMAT_R8_UNORM,                   VK_FORMAT_R8_UNORM,                 GL_R8,                                  1u,  8u),  
	GFX_FORMAT_ATTRIBUTE(R8_UInt,                  DXGI_FORMAT_R8_UINT,                    VK_FORMAT_R8_UINT,                  GL_R8UI,                                1u,  8u),  
	GFX_FORMAT_ATTRIBUTE(R8_SNorm,                 DXGI_FORMAT_R8_SNORM,                   VK_FORMAT_R8_SNORM,                 GL_R8_SNORM,                            1u,  8u),  
	GFX_FORMAT_ATTRIBUTE(R8_SInt,                  DXGI_FORMAT_R8_SINT,                    VK_FORMAT_R8_SINT,                  GL_R8I,                                 1u,  8u),  
	GFX_FORMAT_ATTRIBUTE(A8_UNorm,                 DXGI_FORMAT_A8_UNORM,                   VK_FORMAT_UNDEFINED,                0u,                                     1u,  8u),  
	GFX_FORMAT_ATTRIBUTE(R1_UNorm,                 DXGI_FORMAT_R1_UNORM,                   VK_FORMAT_UNDEFINED,                0u,                                     0u,  1u),  
	GFX_FORMAT_ATTRIBUTE(RGB9E5_SharedXP,          DXGI_FORMAT_R9G9B9E5_SHAREDEXP,         VK_FORMAT_UNDEFINED,                GL_RGB9_E5,                             4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RG8_BG8_UNorm,            DXGI_FORMAT_R8G8_B8G8_UNORM,            VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(GR8_GB8_UNorm,            DXGI_FORMAT_G8R8_G8B8_UNORM,            VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(BC1_Typeless,             DXGI_FORMAT_BC1_TYPELESS,               VK_FORMAT_BC1_RGB_UNORM_BLOCK,      GL_COMPRESSED_RGB_S3TC_DXT1_EXT,        8u,  4u),  
	GFX_FORMAT_ATTRIBUTE(BC1_UNorm,                DXGI_FORMAT_BC1_UNORM,                  VK_FORMAT_BC1_RGB_UNORM_BLOCK,      GL_COMPRESSED_RGB_S3TC_DXT1_EXT,        8u,  4u),  
	GFX_FORMAT_ATTRIBUTE(BC1_UNorm_SRGB,           DXGI_FORMAT_BC1_UNORM_SRGB,             VK_FORMAT_BC1_RGB_SRGB_BLOCK,       GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,       8u,  4u),  
	GFX_FORMAT_ATTRIBUTE(BC2_Typeless,             DXGI_FORMAT_BC2_TYPELESS,               VK_FORMAT_BC2_UNORM_BLOCK,          GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC2_UNorm,                DXGI_FORMAT_BC2_UNORM,                  VK_FORMAT_BC2_UNORM_BLOCK,          GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC2_UNorm_SRGB,           DXGI_FORMAT_BC2_UNORM_SRGB,             VK_FORMAT_BC2_SRGB_BLOCK,           GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, 16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC3_Typeless,             DXGI_FORMAT_BC3_TYPELESS,               VK_FORMAT_BC3_UNORM_BLOCK,          GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC3_UNorm,                DXGI_FORMAT_BC3_UNORM,                  VK_FORMAT_BC3_UNORM_BLOCK,          GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC3_UNorm_SRGB,           DXGI_FORMAT_BC3_UNORM_SRGB,             VK_FORMAT_BC3_SRGB_BLOCK,           GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, 16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC4_Typeless,             DXGI_FORMAT_BC4_TYPELESS,               VK_FORMAT_BC4_UNORM_BLOCK,          GL_COMPRESSED_RED_RGTC1,                8u,  4u),  
	GFX_FORMAT_ATTRIBUTE(BC4_UNorm,                DXGI_FORMAT_BC4_UNORM,                  VK_FORMAT_BC4_UNORM_BLOCK,          GL_COMPRESSED_RED_RGTC1,                8u,  4u),  
	GFX_FORMAT_ATTRIBUTE(BC4_SNorm,                DXGI_FORMAT_BC4_SNORM,                  VK_FORMAT_BC4_UNORM_BLOCK,          GL_COMPRESSED_RED_RGTC1,                8u,  4u),  
	GFX_FORMAT_ATTRIBUTE(BC5_Typeless,             DXGI_FORMAT_BC5_TYPELESS,               VK_FORMAT_BC5_UNORM_BLOCK,          GL_COMPRESSED_RG_RGTC2,                 16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC5_UNorm,                DXGI_FORMAT_BC5_UNORM,                  VK_FORMAT_BC5_UNORM_BLOCK,          GL_COMPRESSED_RG_RGTC2,                 16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC5_SNorm,                DXGI_FORMAT_BC5_SNORM,                  VK_FORMAT_BC5_SNORM_BLOCK,          GL_COMPRESSED_SIGNED_RG_RGTC2,          16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(B5G6R5_UNorm,             DXGI_FORMAT_B5G6R5_UNORM,               VK_FORMAT_B5G6R5_UNORM_PACK16,      0u,                                     2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(BGR5A1_UNorm,             DXGI_FORMAT_B5G5R5A1_UNORM,             VK_FORMAT_B5G5R5A1_UNORM_PACK16,    0u,                                     2u,  16u), 
	GFX_FORMAT_ATTRIBUTE(BGRA8_Typeless,           DXGI_FORMAT_B8G8R8A8_TYPELESS,          VK_FORMAT_B8G8R8A8_UNORM,           0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(BGRA8_UNorm,              DXGI_FORMAT_B8G8R8A8_UNORM,             VK_FORMAT_B8G8R8A8_UNORM,           0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(BGRA8_UNorm_SRGB,         DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,        VK_FORMAT_B8G8R8A8_SRGB,            0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(BGRX8_Typeless,           DXGI_FORMAT_B8G8R8X8_TYPELESS,          VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(BGRX8_UNorm,              DXGI_FORMAT_B8G8R8X8_UNORM,             VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(BGRX8_UNorm_SRGB,         DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,        VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(RGB10_XR_Bias_A2_UNorm,   DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u),
	GFX_FORMAT_ATTRIBUTE(BC6H_Typeless,            DXGI_FORMAT_BC6H_TYPELESS,              VK_FORMAT_BC6H_UFLOAT_BLOCK,        GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,  16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC6H_UF16,                DXGI_FORMAT_BC6H_UF16,                  VK_FORMAT_BC6H_UFLOAT_BLOCK,        GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,  16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC6H_SF16,                DXGI_FORMAT_BC6H_SF16,                  VK_FORMAT_BC6H_SFLOAT_BLOCK,        GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,    16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC7_Typeless,             DXGI_FORMAT_BC7_TYPELESS,               VK_FORMAT_BC7_UNORM_BLOCK,          GL_COMPRESSED_RGBA_BPTC_UNORM,          16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC7_UNorm,                DXGI_FORMAT_BC7_UNORM,                  VK_FORMAT_BC7_UNORM_BLOCK,          GL_COMPRESSED_RGBA_BPTC_UNORM,          16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(BC7_UNorm_SRGB,           DXGI_FORMAT_BC7_UNORM_SRGB,             VK_FORMAT_BC7_SRGB_BLOCK,           GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,    16u, 8u),  
	GFX_FORMAT_ATTRIBUTE(AYUV,                     DXGI_FORMAT_AYUV,                       VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(Y410,                     DXGI_FORMAT_Y410,                       VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(Y416,                     DXGI_FORMAT_Y416,                       VK_FORMAT_UNDEFINED,                0u,                                     8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(NV12,                     DXGI_FORMAT_NV12,                       VK_FORMAT_UNDEFINED,                0u,                                     2u,  12u), 
	GFX_FORMAT_ATTRIBUTE(P010,                     DXGI_FORMAT_P010,                       VK_FORMAT_UNDEFINED,                0u,                                     4u,  24u), 
	GFX_FORMAT_ATTRIBUTE(P016,                     DXGI_FORMAT_P016,                       VK_FORMAT_UNDEFINED,                0u,                                     4u,  24u), 
	GFX_FORMAT_ATTRIBUTE(Opaque_420,               DXGI_FORMAT_420_OPAQUE,                 VK_FORMAT_UNDEFINED,                0u,                                     2u,  12u), 
	GFX_FORMAT_ATTRIBUTE(YUY2,                     DXGI_FORMAT_YUY2,                       VK_FORMAT_UNDEFINED,                0u,                                     4u,  32u), 
	GFX_FORMAT_ATTRIBUTE(Y210,                     DXGI_FORMAT_Y210,                       VK_FORMAT_UNDEFINED,                0u,                                     8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(Y216,                     DXGI_FORMAT_Y216,                       VK_FORMAT_UNDEFINED,                0u,                                     8u,  64u), 
	GFX_FORMAT_ATTRIBUTE(NV11,                     DXGI_FORMAT_NV11,                       VK_FORMAT_UNDEFINED,                0u,                                     0u,  12u)
};

FormatAttribute FormatAttribute::Attribute(EFormat Format)
{
	assert(FormatAttributes[static_cast<uint32_t>(Format)].Format == Format);
	return FormatAttributes[static_cast<uint32_t>(Format)];
}

FormatAttribute FormatAttribute::Attribute(const char8_t* const Format)
{
	for (auto& attr : FormatAttributes)
	{
		if (_stricmp(attr.Name, Format) == 0)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

FormatAttribute FormatAttribute::Attribute_Vk(uint32_t Format)
{
	for (auto& attr : FormatAttributes)
	{
		if (attr.VkFormat == Format)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

FormatAttribute FormatAttribute::Attribute_DXGI(uint32_t Format)
{
	for (auto& attr : FormatAttributes)
	{
		if (static_cast<uint32_t>(attr.DXGIFromat) == Format)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

FormatAttribute FormatAttribute::Attribute_GL(uint32_t Format)
{
	for (auto& attr : FormatAttributes)
	{
		if (attr.GLFormat == Format)
		{
			return attr;
		}
	}

	assert(0);
	return FormatAttribute();
}

uint32_t FormatAttribute::ToDXGIFormat(EFormat Format)
{
	assert(FormatAttributes[static_cast<uint32_t>(Format)].Format == Format);
	return FormatAttributes[static_cast<uint32_t>(Format)].DXGIFromat;
}

uint32_t FormatAttribute::ToVulkanFormat(EFormat Format)
{
	assert(FormatAttributes[static_cast<uint32_t>(Format)].Format == Format);
	return FormatAttributes[static_cast<uint32_t>(Format)].VkFormat;
}

EFormat FormatAttribute::ToSRGBFormat(EFormat Format)
{
	switch (Format)
	{
	case EFormat::BC1_UNorm:   return EFormat::BC1_UNorm_SRGB;
	case EFormat::BC2_UNorm:   return EFormat::BC2_UNorm_SRGB;
	case EFormat::BGRA8_UNorm: return EFormat::BGRA8_UNorm_SRGB;
	case EFormat::BGRX8_UNorm: return EFormat::BGRX8_UNorm_SRGB;
	case EFormat::BC7_UNorm:   return EFormat::BC7_UNorm_SRGB;
	case EFormat::RGBA8_UNorm: return EFormat::RGBA8_UNorm_SRGB;
	}
	return Format;
}

bool8_t FormatAttribute::IsColor(EFormat Format)
{
	return !(IsDepth(Format) || IsDepthStenci(Format));
}

bool8_t FormatAttribute::IsDepth(EFormat Format)
{
	return Format == EFormat::D16_UNorm || Format == EFormat::D32_Float;
}

bool8_t FormatAttribute::IsDepthStenci(EFormat Format)
{
	return Format == EFormat::D24_UNorm_S8_UInt || Format == EFormat::D32_Float_S8_UInt;
}

std::pair<uint32_t, uint32_t> FormatAttribute::CalculateFormatBytes(uint32_t Width, uint32_t Height, EFormat Format, __out uint32_t& SliceBytes, __out uint32_t& RowBytes)
{
	bool8_t Compressed =
		Format == EFormat::BC1_Typeless ||
		Format == EFormat::BC1_UNorm ||
		Format == EFormat::BC1_UNorm_SRGB ||
		Format == EFormat::BC2_Typeless ||
		Format == EFormat::BC2_UNorm ||
		Format == EFormat::BC2_UNorm_SRGB ||
		Format == EFormat::BC3_Typeless ||
		Format == EFormat::BC3_UNorm ||
		Format == EFormat::BC3_UNorm_SRGB ||
		Format == EFormat::BC4_Typeless ||
		Format == EFormat::BC4_UNorm ||
		Format == EFormat::BC4_SNorm ||
		Format == EFormat::BC5_Typeless ||
		Format == EFormat::BC5_UNorm ||
		Format == EFormat::BC5_SNorm ||
		Format == EFormat::BC6H_Typeless ||
		Format == EFormat::BC6H_UF16 ||
		Format == EFormat::BC6H_SF16 ||
		Format == EFormat::BC7_Typeless ||
		Format == EFormat::BC7_UNorm ||
		Format == EFormat::BC7_UNorm_SRGB;

	bool8_t Planar = 
		Format == EFormat::NV12 ||
		Format == EFormat::Opaque_420 || 
		Format == EFormat::P010 ||
		Format == EFormat::P016;

	bool8_t Packed = 
		Format == EFormat::RG8_BG8_UNorm ||
		Format == EFormat::GR8_GB8_UNorm ||
		Format == EFormat::YUY2 ||
		Format == EFormat::Y210 ||
		Format == EFormat::Y216;

	auto Attribute = FormatAttribute::Attribute(Format);
	uint32_t Cols = Width;
	uint32_t Rows = Height;
	uint32_t Bytes = Attribute.BytesPerPixel;

	if (Compressed)
	{
		uint32_t BlockWide = 0u;
		if (Width > 0u)
		{
			BlockWide = std::max<uint32_t>(1u, (Width + 3u) / 4u);
		}
		uint32_t BlockHigh = 0u;
		if (Height > 0u)
		{
			BlockHigh = std::max<uint32_t>(1u, (Height + 3u) / 4u);
		}
		RowBytes = BlockWide * Bytes;
		Rows = BlockHigh;
		SliceBytes = RowBytes * BlockHigh;
		Cols = BlockWide;
	}
	else if (Packed)
	{
		RowBytes = ((Width + 1u) >> 1u) * Bytes;
		Rows = Height;
		SliceBytes = RowBytes * Height;
	}
	else if (Format == EFormat::NV11)
	{
		RowBytes = ((Width + 3u) >> 2u) * 4u;
		Rows = Height * 2u;
		SliceBytes = RowBytes * Rows;
	}
	else if (Planar)
	{
		RowBytes = ((Width + 1u) >> 1u) * Bytes;
		SliceBytes = (RowBytes * Height) + ((RowBytes * Height + 1u) >> 1u);
		Rows = Height + ((Height + 1u) >> 1u);
	}
	else
	{
		assert(Attribute.BitsPerPixel);
		RowBytes = (Width * Attribute.BitsPerPixel + 7u) / 8u;
		Rows = Height;
		SliceBytes = RowBytes * Height;
	}

	return std::make_pair(Cols, Rows);
}

NAMESPACE_END(RHI)
