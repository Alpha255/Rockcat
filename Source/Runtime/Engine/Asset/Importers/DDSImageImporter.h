#pragma once

#include "Engine/Asset/ImageAsset.h"
#include <dxgiformat.h>
#include <External/DDS.h>

class DDSImageImporter : public IAssetImporter
{
public:
	DDSImageImporter()
		: IAssetImporter({ AssetType("DirectDraw Surface", ".dds")})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<ImageAsset>(AssetPath); }

	bool Reimport(Asset& InAsset) override final
	{
		auto& Image = Cast<ImageAsset>(InAsset);
#if 0
		assert(Data && Size);
		assert(Size >= (sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)));
		assert(*reinterpret_cast<const uint32_t*>(Data) == DirectX::DDS_MAGIC);

		bool DXT10Header = false;
		auto Header = reinterpret_cast<const DirectX::DDS_HEADER*>(Data + sizeof(uint32_t));
		assert(Header->size == sizeof(DirectX::DDS_HEADER) && Header->ddspf.size == sizeof(DirectX::DDS_PIXELFORMAT));
		if ((Header->ddspf.flags & DDS_FOURCC) && (Header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0')))
		{
			assert(Size >= (sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10)));
			DXT10Header = true;
		}

		auto Offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + (DXT10Header ? sizeof(DirectX::DDS_HEADER_DXT10) : 0u);
		auto BitSize = Size - Offset;
		auto BitData = Data + Offset;

		bool Cubemap = false;

		RHI::ImageDesc Desc;
		Desc.Width = Header->width;
		Desc.Height = Header->height;
		Desc.Depth = Header->depth == 0u ? 1u : Header->depth;
		Desc.MipLevels = Header->mipMapCount == 0u ? 1u : Header->mipMapCount;
		assert(Desc.MipLevels <= D3D11_REQ_MIP_LEVELS);

		if (DXT10Header)
		{
			auto DXTHeader = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const uint8_t*>(Header) + sizeof(DirectX::DDS_HEADER));
			assert(DXTHeader && DXTHeader->arraySize);

			Desc.ArrayLayers = DXTHeader->arraySize;
			assert(DXTHeader->dxgiFormat != DXGI_FORMAT_AI44 &&
				DXTHeader->dxgiFormat != DXGI_FORMAT_IA44 &&
				DXTHeader->dxgiFormat != DXGI_FORMAT_P8 &&
				DXTHeader->dxgiFormat != DXGI_FORMAT_A8P8);
			auto FormatAttr = RHI::FormatAttribute::Attribute_DXGI(DXTHeader->dxgiFormat);
			assert(FormatAttr.BytesPerPixel);

			Desc.Format = FormatAttr.Format;
			switch (DXTHeader->resourceDimension)
			{
			case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
				if (Header->flags & DDS_HEIGHT)
				{
					assert(Desc.Height == 1u);
				}
				Desc.Height = Desc.Depth = 1u;
				Desc.Type = Desc.ArrayLayers > 1u ? RHI::EImageType::T_1D_Array : RHI::EImageType::T_1D;
				break;
			case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
				if (DXTHeader->miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE)
				{
					Desc.Type = Desc.ArrayLayers > 1u ? RHI::EImageType::T_Cube_Array : RHI::EImageType::T_Cube;
					Desc.ArrayLayers *= 6u;
					Cubemap = true;
				}
				else
				{
					Desc.Type = Desc.ArrayLayers > 1u ? RHI::EImageType::T_2D_Array : RHI::EImageType::T_2D;
				}
				Desc.Depth = 1u;
				break;
			case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
				assert(Header->flags & DDS_HEADER_FLAGS_VOLUME);
				Desc.Type = RHI::EImageType::T_3D;
				break;
			case D3D11_RESOURCE_DIMENSION_BUFFER:
				Desc.Type = RHI::EImageType::Buffer;
				break;
			}
		}
		else
		{
			auto FormatAttr = RHI::FormatAttribute::Attribute_DXGI(PixelFormatToDXGIFormat(Header->ddspf));
			assert(FormatAttr.Format != RHI::EFormat::Unknown && FormatAttr.BytesPerPixel);
			Desc.Format = FormatAttr.Format;

			if (Header->flags & DDS_HEADER_FLAGS_VOLUME)
			{
				Desc.Type = RHI::EImageType::T_3D;
			}
			else
			{
				if (Header->caps2 & DDS_CUBEMAP)
				{
					assert((Header->caps2 & DDS_CUBEMAP_ALLFACES) == DDS_CUBEMAP_ALLFACES);
					Desc.ArrayLayers = 6u;
					Cubemap = true;
					Desc.Type = RHI::EImageType::T_Cube;
				}
				else
				{
					Desc.Type = Desc.ArrayLayers > 1u ? RHI::EImageType::T_2D_Array : RHI::EImageType::T_2D;
				}
			}
		}

		switch (Desc.Type)
		{
		case RHI::EImageType::T_2D:
		case RHI::EImageType::T_2D_Array:
			assert(
				Desc.ArrayLayers <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
				Desc.Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION &&
				Desc.Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
			break;
		case RHI::EImageType::T_Cube:
		case RHI::EImageType::T_Cube_Array:
			assert(
				Desc.ArrayLayers <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION &&
				Desc.Width <= D3D11_REQ_TEXTURECUBE_DIMENSION &&
				Desc.Height <= D3D11_REQ_TEXTURECUBE_DIMENSION);
			break;
		case RHI::EImageType::T_3D:
			assert(
				Desc.ArrayLayers == 1u &&
				Desc.Width <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
				Desc.Height <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION &&
				Desc.Depth <= D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION);
			break;
		}

		/// gen mipmaps
		Desc.Asset.Data = BitData;
		Desc.Asset.Size = static_cast<uint32_t>(BitSize);
		Desc.Subresources.resize(static_cast<size_t>(Desc.MipLevels) * static_cast<size_t>(Desc.ArrayLayers));
		uint32_t Index = 0u;
		for (uint32_t ArrayIndex = 0u; ArrayIndex < Desc.ArrayLayers; ++ArrayIndex)
		{
			uint32_t Width = Desc.Width;
			uint32_t Height = Desc.Height;
			uint32_t Depth = Desc.Depth;
			for (uint32_t MipIndex = 0u; MipIndex < Desc.MipLevels; ++MipIndex)
			{
				Desc.Subresources[Index].Width = Width;
				Desc.Subresources[Index].Height = Height;
				Desc.Subresources[Index].Depth = Depth;
				Desc.Subresources[Index].MipLevel = MipIndex;
				Desc.Subresources[Index].ArrayLayer = ArrayIndex;

				RHI::FormatAttribute::CalculateFormatBytes(Width, Height, Desc.Format, Desc.Subresources[Index].SliceBytes, Desc.Subresources[Index].RowBytes);
				assert((BitData + static_cast<size_t>(Desc.Subresources[Index].SliceBytes) * static_cast<size_t>(Depth)) <= (BitData + BitSize));
				Desc.Subresources[Index].Offset = Index == 0u ? 0u : Desc.Subresources[Index].Offset + Desc.Subresources[Index].SliceBytes * Depth;

				Width = Width >> 1;
				Height = Height >> 1;
				Depth = Depth >> 1;
				Width = Width == 0u ? 1u : Width;
				Height = Height == 0u ? 1u : Height;
				Depth = Depth == 0u ? 1u : Depth;

				++Index;
			}
		}

		return Desc;
#endif

		return false;
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
