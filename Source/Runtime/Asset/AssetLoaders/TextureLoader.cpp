#include "Asset/AssetLoaders/TextureLoader.h"
#include "Asset/Texture.h"
#include "Services/SpdLogService.h"

#include <Asset/DDS.h>
#include <dxgiformat.h>

#pragma warning(disable:4244)
#include <Submodules/stb/stb_image.h>
#pragma warning(default:4244)

static DXGI_FORMAT GetDXGIFormat(const DirectX::DDS_PIXELFORMAT& PixelFormat)
{
#define GET_FORMAT_BITMASK(R, G, B, A, Format) \
	if (PixelFormat.RBitMask == R && PixelFormat.GBitMask == G && PixelFormat.BBitMask == B && PixelFormat.ABitMask == A) \
	{ \
		return Format; \
	} else

#define GET_FORMAT_FOURCC(ch0, ch1, ch2, ch3, Format) \
	if (MAKEFOURCC(ch0, ch1, ch2, ch3) == PixelFormat.fourCC) \
	{ \
		return Format; \
	} else

#define GET_FORMAT_END {}

	if (PixelFormat.flags & DDS_RGB)
	{
		switch (PixelFormat.RGBBitCount)
		{
		case 32u:
			GET_FORMAT_BITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000, DXGI_FORMAT_R8G8B8A8_UNORM)
			GET_FORMAT_BITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, DXGI_FORMAT_B8G8R8A8_UNORM)
			GET_FORMAT_BITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0, DXGI_FORMAT_B8G8R8X8_UNORM)
			GET_FORMAT_BITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000, DXGI_FORMAT_R10G10B10A2_UNORM)
			GET_FORMAT_BITMASK(0x0000ffff, 0xffff0000, 0, 0, DXGI_FORMAT_R16G16_UNORM)
			GET_FORMAT_BITMASK(0xffffffff, 0, 0, 0, DXGI_FORMAT_R32_FLOAT) // D3DX writes this out as a FourCC of 114
			GET_FORMAT_END
			break;
		case 24:
			break;
		case 16:
			GET_FORMAT_BITMASK(0x7c00, 0x03e0, 0x001f, 0x8000, DXGI_FORMAT_B5G5R5A1_UNORM)
			GET_FORMAT_BITMASK(0xf800, 0x07e0, 0x001f, 0, DXGI_FORMAT_B5G6R5_UNORM)
			GET_FORMAT_BITMASK(0x0f00, 0x00f0, 0x000f, 0xf000, DXGI_FORMAT_B4G4R4A4_UNORM)
			GET_FORMAT_END
			break;
		}
	}
	else if (PixelFormat.flags & DDS_LUMINANCE)
	{
		if (8 == PixelFormat.RGBBitCount)
		{
			GET_FORMAT_BITMASK(0xff, 0, 0, 0, DXGI_FORMAT_R8_UNORM) // D3DX10/11 writes this out as DX10 extension
			GET_FORMAT_BITMASK(0x00ff, 0, 0, 0xff00, DXGI_FORMAT_R8G8_UNORM) // Some DDS writers assume the bitcount should be 8 instead of 16
			GET_FORMAT_END
		}
		if (16 == PixelFormat.RGBBitCount)
		{
			GET_FORMAT_BITMASK(0xffff, 0, 0, 0, DXGI_FORMAT_R16_UNORM) // D3DX10/11 writes this out as DX10 extension
			GET_FORMAT_BITMASK(0x00ff, 0, 0, 0xff00, DXGI_FORMAT_R8G8_UNORM) // D3DX10/11 writes this out as DX10 extension
			GET_FORMAT_END
		}
	}
	else if (PixelFormat.flags & DDS_ALPHA)
	{
		if (8 == PixelFormat.RGBBitCount)
		{
			return DXGI_FORMAT_A8_UNORM;
		}
	}
	else if (PixelFormat.flags & DDS_BUMPDUDV)
	{
		if (16 == PixelFormat.RGBBitCount)
		{
			GET_FORMAT_BITMASK(0x00ff, 0xff00, 0, 0, DXGI_FORMAT_R8G8_SNORM)
			GET_FORMAT_END
		}

		if (32 == PixelFormat.RGBBitCount)
		{
			GET_FORMAT_BITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000, DXGI_FORMAT_R8G8B8A8_SNORM) // D3DX10/11 writes this out as DX10 extension
			GET_FORMAT_BITMASK(0x0000ffff, 0xffff0000, 0, 0, DXGI_FORMAT_R16G16_SNORM) // D3DX10/11 writes this out as DX10 extension
			GET_FORMAT_END
		}
	}
	else if (PixelFormat.flags & DDS_FOURCC)
	{
		GET_FORMAT_FOURCC('D', 'X', 'T', '1', DXGI_FORMAT_BC1_UNORM)
		GET_FORMAT_FOURCC('D', 'X', 'T', '3', DXGI_FORMAT_BC2_UNORM)
		GET_FORMAT_FOURCC('D', 'X', 'T', '5', DXGI_FORMAT_BC3_UNORM)
		GET_FORMAT_FOURCC('D', 'X', 'T', '2', DXGI_FORMAT_BC2_UNORM)
		GET_FORMAT_FOURCC('D', 'X', 'T', '4', DXGI_FORMAT_BC3_UNORM)
		GET_FORMAT_FOURCC('A', 'T', 'I', '1', DXGI_FORMAT_BC4_UNORM)
		GET_FORMAT_FOURCC('B', 'C', '4', 'U', DXGI_FORMAT_BC4_UNORM)
		GET_FORMAT_FOURCC('B', 'C', '4', 'S', DXGI_FORMAT_BC4_SNORM)
		GET_FORMAT_FOURCC('A', 'T', 'I', '2', DXGI_FORMAT_BC5_UNORM)
		GET_FORMAT_FOURCC('B', 'C', '5', 'U', DXGI_FORMAT_BC5_UNORM)
		GET_FORMAT_FOURCC('B', 'C', '5', 'S', DXGI_FORMAT_BC5_SNORM)
		GET_FORMAT_FOURCC('R', 'G', 'B', 'G', DXGI_FORMAT_R8G8_B8G8_UNORM)
		GET_FORMAT_FOURCC('G', 'R', 'G', 'B', DXGI_FORMAT_G8R8_G8B8_UNORM)
		GET_FORMAT_FOURCC('Y', 'U', 'Y', '2', DXGI_FORMAT_YUY2)
		GET_FORMAT_END

		switch (PixelFormat.fourCC)
		{
		case 36:  return DXGI_FORMAT_R16G16B16A16_UNORM;
		case 110: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case 111: return DXGI_FORMAT_R16_FLOAT;
		case 112: return DXGI_FORMAT_R16G16_FLOAT;
		case 113: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case 114: return DXGI_FORMAT_R32_FLOAT;
		case 115: return DXGI_FORMAT_R32G32_FLOAT;
		case 116: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;

#undef GET_FORMAT_BITMASK
#undef GET_FORMAT_FOURCC
#undef GET_FORMAT_END
}

TextureLoader::TextureLoader()
	: AssetLoader({
		".jpeg",
		".jpg",
		".png",
		".bmp",
		".tga",
		".psd",
		".hdr",
		GetDDSExtension()
	})
{
	LOG_INFO_CAT(LogAsset, "Use stb_image @2.3");
}

std::shared_ptr<Asset> TextureLoader::CreateAsset(const std::filesystem::path& Path)
{
	return std::make_shared<Texture>(Path);
}

bool TextureLoader::Load(Asset& Target)
{
	auto& Image = Cast<Texture>(Target);
	Image.GetDesc().SetBulkData(Image.LoadData());

	return IsDDSTexture(Target.GetExtension()) ? LoadDDS(Image) : LoadStb(Image);

}

bool TextureLoader::LoadStb(Texture& Image)
{
	auto& Desc = Image.GetDesc();

	auto const DataSize = static_cast<int32_t>(Desc.BulkData->GetSize());
	auto Data = reinterpret_cast<const stbi_uc*>(Desc.BulkData->GetRawData());

	int32_t Width = 0, Height = 0, Channels = 0, OriginalChannels = STBI_default;

	if (!stbi_info_from_memory(Data, DataSize, &Width, &Height, &OriginalChannels))
	{
		LOG_ERROR_CAT(LogAsset, "Couldn't parse image header of \"{}\": {}", Image.GetPath().string(), stbi_failure_reason());
		return false;
	}

	stbi_uc* Bitmap = nullptr;
	bool IsHDR = stbi_is_hdr_from_memory(Data, DataSize);
	Channels = OriginalChannels == STBI_rgb ? STBI_rgb_alpha : OriginalChannels;

	if (IsHDR)
	{
		if (auto Pixels = stbi_loadf_from_memory(Data, DataSize, &Width, &Height, &OriginalChannels, Channels))
		{
			Bitmap = reinterpret_cast<stbi_uc*>(Pixels);
		}
	}
	else
	{
		Bitmap = stbi_load_from_memory(Data, DataSize, &Width, &Height, &OriginalChannels, Channels);
	}

	if (!Bitmap)
	{
		LOG_ERROR_CAT(LogAsset, "Failed to load image \"{}\": {}", Image.GetPath().string(), stbi_failure_reason());
		return false;
	}

	Desc.SetWidth(Width)
		.SetHeight(Height)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(Desc.IsLinear ? ERHIFormat::RGBA8_UNorm : ERHIFormat::RGBA8_UNorm_SRGB)
		.SetUsages(ERHIBufferUsageFlags::ShaderResource)
		.SetPermanentState(ERHIResourceState::ShaderResource)
		.SetName(Image.GetName());

	return true;
}

bool TextureLoader::LoadDDS(Texture& Image)
{
	auto& Desc = Image.GetDesc();

	auto const DataSize = static_cast<int32_t>(Desc.BulkData->GetSize());
	auto Data = reinterpret_cast<const uint8_t*>(Desc.BulkData->GetRawData());

	assert(DataSize >= (sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)));
	assert(*reinterpret_cast<const uint32_t*>(Data) == DirectX::DDS_MAGIC);

	bool DXT10Header = false;
	auto Header = reinterpret_cast<const DirectX::DDS_HEADER*>(Data + sizeof(uint32_t));
	assert(Header->size == sizeof(DirectX::DDS_HEADER) && Header->ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));
	if ((Header->ddspf.flags & DDS_FOURCC) && (Header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0')))
	{
		assert(DataSize >= (sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10)));
		DXT10Header = true;
	}

	auto Offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + (DXT10Header ? sizeof(DirectX::DDS_HEADER_DXT10) : 0u);
	auto BitSize = DataSize - Offset;
	auto BitData = Data + Offset;
	bool Cubemap = false;

	Desc.SetWidth(Header->width)
		.SetHeight(Header->height)
		.SetDepth(Header->depth == 0u ? 1u : Header->depth)
		.SetNumMipLevel(Header->mipMapCount == 0u ? 1u : Header->mipMapCount)
		.SetUsages(ERHIBufferUsageFlags::ShaderResource)
		.SetPermanentState(ERHIResourceState::ShaderResource)
		.SetBulkData(BitSize, BitData);
	assert(Desc.NumMipLevel <= D3D11_REQ_MIP_LEVELS);

	if (DXT10Header)
	{
		auto DXTHeader = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const uint8_t*>(Header) + sizeof(DirectX::DDS_HEADER));
		assert(DXTHeader && DXTHeader->arraySize);

		Desc.SetNumArrayLayer(DXTHeader->arraySize);
		assert(DXTHeader->dxgiFormat != DXGI_FORMAT_AI44 &&
			DXTHeader->dxgiFormat != DXGI_FORMAT_IA44 &&
			DXTHeader->dxgiFormat != DXGI_FORMAT_P8 &&
			DXTHeader->dxgiFormat != DXGI_FORMAT_A8P8);

		Desc.SetFormat(RHI::GetRHIFormat(DXTHeader->dxgiFormat));
		switch (DXTHeader->resourceDimension)
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			if (Header->flags & DDS_HEIGHT)
			{
				assert(Desc.Height == 1u);
			}
			Desc.SetHeight(1u)
				.SetDepth(1u)
				.SetDimension(Desc.NumArrayLayer > 1u ? ERHITextureDimension::T_1D_Array : ERHITextureDimension::T_1D);
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			if (DXTHeader->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				Desc.SetDimension(Desc.NumArrayLayer > 1u ? ERHITextureDimension::T_Cube_Array : ERHITextureDimension::T_Cube)
					.SetNumArrayLayer(Desc.NumArrayLayer * 6u);
				Cubemap = true;
			}
			else
			{
				Desc.SetDimension(Desc.NumArrayLayer > 1u ? ERHITextureDimension::T_2D_Array : ERHITextureDimension::T_2D);
			}
			Desc.SetDepth(1u);
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			assert(Header->flags & DDS_HEADER_FLAGS_VOLUME);
			Desc.SetDimension(ERHITextureDimension::T_3D);
			break;
		case D3D11_RESOURCE_DIMENSION_BUFFER:
			Desc.SetDimension(ERHITextureDimension::Buffer);
			break;
		}
	}
	else
	{
		Desc.SetFormat(RHI::GetRHIFormat(GetDXGIFormat(Header->ddspf)));

		if (Header->flags & DDS_HEADER_FLAGS_VOLUME)
		{
			Desc.SetDimension(ERHITextureDimension::T_3D);
		}
		else
		{
			if (Header->caps2 & DDS_CUBEMAP)
			{
				assert((Header->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
				Cubemap = true;
				Desc.SetDimension(ERHITextureDimension::T_Cube)
					.SetNumArrayLayer(Desc.NumArrayLayer * 6u);
			}
			else
			{
				Desc.SetDimension(Desc.NumArrayLayer > 1u ? ERHITextureDimension::T_2D_Array : ERHITextureDimension::T_2D);
			}
		}
	}

	switch (Desc.Dimension)
	{
	case ERHITextureDimension::T_2D:
	case ERHITextureDimension::T_2D_Array:
		assert(
			Desc.NumArrayLayer <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
			Desc.Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION &&
			Desc.Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		break;
	case ERHITextureDimension::T_Cube:
	case ERHITextureDimension::T_Cube_Array:
		assert(
			Desc.NumArrayLayer <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
			Desc.Width <= D3D11_REQ_TEXTURECUBE_DIMENSION &&
			Desc.Height <= D3D11_REQ_TEXTURECUBE_DIMENSION);
		break;
	case ERHITextureDimension::T_3D:
		assert(
			Desc.NumArrayLayer == 1u &&
			Desc.Width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
			Desc.Height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
			Desc.Depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION);
		break;
	}

	return false;
}