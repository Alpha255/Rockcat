#pragma once

#include "Engine/Asset/TextureAsset.h"
#include "Engine/Services/RenderService.h"
#include "Engine/RHI/RHIBackend.h"

#include <dxgiformat.h>
#include <External/DDS.h>


class DDSImageImporter : public IAssetImporter
{
public:
	DDSImageImporter()
		: IAssetImporter({ AssetType("DirectDraw Surface", ".dds")})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<TextureAsset>(AssetPath); }

	bool Reimport(Asset& InAsset, const AssetType& InAssetType) override final
	{
		auto& Image = Cast<TextureAsset>(InAsset);
		auto& RawData = Image.GetRawData(InAssetType.ContentsType);

		auto const DataSize = static_cast<int32_t>(RawData.Size);
		auto Data = reinterpret_cast<const uint8_t*>(RawData.Data.get());

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
		RHITextureCreateInfo CreateInfo;
		CreateInfo.SetWidth(Header->width)
			.SetHeight(Header->height)
			.SetDepth(Header->depth == 0u ? 1u : Header->depth)
			.SetNumMipLevel(Header->mipMapCount == 0u ? 1u : Header->mipMapCount)
			.SetUsages(ERHIBufferUsageFlags::ShaderResource)
			.SetPermanentState(ERHIResourceState::ShaderResource);
		assert(CreateInfo.NumMipLevel <= D3D11_REQ_MIP_LEVELS);

		if (DXT10Header)
		{
			auto DXTHeader = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const uint8_t*>(Header) + sizeof(DirectX::DDS_HEADER));
			assert(DXTHeader && DXTHeader->arraySize);

			CreateInfo.SetNumArrayLayer(DXTHeader->arraySize);
			assert(DXTHeader->dxgiFormat != DXGI_FORMAT_AI44 &&
				DXTHeader->dxgiFormat != DXGI_FORMAT_IA44 &&
				DXTHeader->dxgiFormat != DXGI_FORMAT_P8 &&
				DXTHeader->dxgiFormat != DXGI_FORMAT_A8P8);

			CreateInfo.SetFormat(RHI::GetRHIFormat(DXTHeader->dxgiFormat));
			switch (DXTHeader->resourceDimension)
			{
			case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
				if (Header->flags & DDS_HEIGHT)
				{
					assert(CreateInfo.Height == 1u);
				}
				CreateInfo.SetHeight(1u)
					.SetDepth(1u)
					.SetDimension(CreateInfo.NumArrayLayer > 1u ? ERHITextureDimension::T_1D_Array : ERHITextureDimension::T_1D);
				break;
			case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
				if (DXTHeader->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
				{
					CreateInfo.SetDimension(CreateInfo.NumArrayLayer > 1u ? ERHITextureDimension::T_Cube_Array : ERHITextureDimension::T_Cube)
						.SetNumArrayLayer(CreateInfo.NumArrayLayer * 6u);
					Cubemap = true;
				}
				else
				{
					CreateInfo.SetDimension(CreateInfo.NumArrayLayer > 1u ? ERHITextureDimension::T_2D_Array : ERHITextureDimension::T_2D);
				}
				CreateInfo.SetDepth(1u);
				break;
			case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
				assert(Header->flags & DDS_HEADER_FLAGS_VOLUME);
				CreateInfo.SetDimension(ERHITextureDimension::T_3D);
				break;
			case D3D11_RESOURCE_DIMENSION_BUFFER:
				CreateInfo.SetDimension(ERHITextureDimension::Buffer);
				break;
			}
		}
		else
		{
			CreateInfo.SetFormat(RHI::GetRHIFormat(GetDXGIFormat(Header->ddspf)));

			if (Header->flags & DDS_HEADER_FLAGS_VOLUME)
			{
				CreateInfo.SetDimension(ERHITextureDimension::T_3D);
			}
			else
			{
				if (Header->caps2 & DDS_CUBEMAP)
				{
					assert((Header->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
					Cubemap = true;
					CreateInfo.SetDimension(ERHITextureDimension::T_Cube)
						.SetNumArrayLayer(CreateInfo.NumArrayLayer * 6u);
				}
				else
				{
					CreateInfo.SetDimension(CreateInfo.NumArrayLayer > 1u ? ERHITextureDimension::T_2D_Array : ERHITextureDimension::T_2D);
				}
			}
		}

		switch (CreateInfo.Dimension)
		{
		case ERHITextureDimension::T_2D:
		case ERHITextureDimension::T_2D_Array:
			assert(
				CreateInfo.NumArrayLayer <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
				CreateInfo.Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION &&
				CreateInfo.Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
			break;
		case ERHITextureDimension::T_Cube:
		case ERHITextureDimension::T_Cube_Array:
			assert(
				CreateInfo.NumArrayLayer <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
				CreateInfo.Width <= D3D11_REQ_TEXTURECUBE_DIMENSION &&
				CreateInfo.Height <= D3D11_REQ_TEXTURECUBE_DIMENSION);
			break;
		case ERHITextureDimension::T_3D:
			assert(
				CreateInfo.NumArrayLayer == 1u &&
				CreateInfo.Width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
				CreateInfo.Height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
				CreateInfo.Depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION);
			break;
		}

		CreateInfo.SetInitialData(DataBlock(BitSize, BitData));
		Image.CreateRHI(RenderService::Get().GetBackend().GetDevice(), CreateInfo);

		return true;
	}
private:
	static DXGI_FORMAT GetDXGIFormat(const DirectX::DDS_PIXELFORMAT& PixelFormat)
	{
#define IS_BITMASK(R, G, B, A) (PixelFormat.RBitMask == R && PixelFormat.GBitMask == G && PixelFormat.BBitMask == B && PixelFormat.ABitMask == A)
		if (PixelFormat.flags & DDS_RGB)
		{
			switch (PixelFormat.RGBBitCount)
			{
			case 32:
				if (IS_BITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				{
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				}
				if (IS_BITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
				{
					return DXGI_FORMAT_B8G8R8A8_UNORM;
				}
				if (IS_BITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0))
				{
					return DXGI_FORMAT_B8G8R8X8_UNORM;
				}
				if (IS_BITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
				{
					return DXGI_FORMAT_R10G10B10A2_UNORM;
				}
				if (IS_BITMASK(0x0000ffff, 0xffff0000, 0, 0))
				{
					return DXGI_FORMAT_R16G16_UNORM;
				}
				if (IS_BITMASK(0xffffffff, 0, 0, 0))
				{
					return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
				}
				break;
			case 24:
				break;
			case 16:
				if (IS_BITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
				{
					return DXGI_FORMAT_B5G5R5A1_UNORM;
				}
				if (IS_BITMASK(0xf800, 0x07e0, 0x001f, 0))
				{
					return DXGI_FORMAT_B5G6R5_UNORM;
				}
				if (IS_BITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
				{
					return DXGI_FORMAT_B4G4R4A4_UNORM;
				}
				break;
			}
		}
		else if (PixelFormat.flags & DDS_LUMINANCE)
		{
			if (8 == PixelFormat.RGBBitCount)
			{
				if (IS_BITMASK(0xff, 0, 0, 0))
				{
					return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
				}
				if (IS_BITMASK(0x00ff, 0, 0, 0xff00))
				{
					return DXGI_FORMAT_R8G8_UNORM; // Some DDS writers assume the bitcount should be 8 instead of 16
				}
			}
			if (16 == PixelFormat.RGBBitCount)
			{
				if (IS_BITMASK(0xffff, 0, 0, 0))
				{
					return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
				}
				if (IS_BITMASK(0x00ff, 0, 0, 0xff00))
				{
					return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
				}
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
				if (IS_BITMASK(0x00ff, 0xff00, 0, 0))
				{
					return DXGI_FORMAT_R8G8_SNORM; // D3DX10/11 writes this out as DX10 extension
				}
			}

			if (32 == PixelFormat.RGBBitCount)
			{
				if (IS_BITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				{
					return DXGI_FORMAT_R8G8B8A8_SNORM; // D3DX10/11 writes this out as DX10 extension
				}
				if (IS_BITMASK(0x0000ffff, 0xffff0000, 0, 0))
				{
					return DXGI_FORMAT_R16G16_SNORM; // D3DX10/11 writes this out as DX10 extension
				}
			}
		}
		else if (PixelFormat.flags & DDS_FOURCC)
		{
			if (MAKEFOURCC('D', 'X', 'T', '1') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC1_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '3') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC2_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '5') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC3_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '2') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC2_UNORM;
			}
			if (MAKEFOURCC('D', 'X', 'T', '4') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC3_UNORM;
			}
			if (MAKEFOURCC('A', 'T', 'I', '1') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC4_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '4', 'U') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC4_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '4', 'S') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC4_SNORM;
			}
			if (MAKEFOURCC('A', 'T', 'I', '2') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC5_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '5', 'U') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC5_UNORM;
			}
			if (MAKEFOURCC('B', 'C', '5', 'S') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_BC5_SNORM;
			}
			if (MAKEFOURCC('R', 'G', 'B', 'G') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_R8G8_B8G8_UNORM;
			}
			if (MAKEFOURCC('G', 'R', 'G', 'B') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_G8R8_G8B8_UNORM;
			}
			if (MAKEFOURCC('Y', 'U', 'Y', '2') == PixelFormat.fourCC)
			{
				return DXGI_FORMAT_YUY2;
			}
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
#undef IS_BITMASK
	}
};
