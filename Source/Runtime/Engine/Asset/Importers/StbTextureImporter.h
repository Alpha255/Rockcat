#pragma once

#include "Engine/Asset/TextureAsset.h"
#include "Engine/RHI/RHITexture.h"
#include "Engine/Services/SpdLogService.h"
#pragma warning(disable:4244)
#include <Submodules/stb/stb_image.h>
#pragma warning(default:4244)

class StbImageImporter : public IAssetImporter
{
public:
	StbImageImporter()
		: IAssetImporter({
				AssetType("Joint Photographic Experts Group", ".jpeg"),
				AssetType("Joint Photographic Experts Group", ".jpg"),
				AssetType("Portable Network Graphics", ".png"), 
				AssetType("Bitmap", ".bmp"), 
				AssetType("Targa", ".tga"), 
				AssetType("Photoshop Document", ".psd"),
				AssetType("High-Dynamic Range", ".hdr")})
	{
		LOG_INFO("Create STB image importer, stb_image version: 2.3");
	}

	std::shared_ptr<Asset> CreateAsset(std::filesystem::path&& AssetPath) override final { return std::make_shared<TextureAsset>(std::move(AssetPath)); }

	bool Reimport(Asset& InAsset) override final
	{
		auto& Image = Cast<TextureAsset>(InAsset);

		auto DataSize = static_cast<int32_t>(Image.GetRawData().Size);
		auto Data = reinterpret_cast<const stbi_uc*>(Image.GetRawData().Data.get());

		int32_t Width = 0, Height = 0, Channels = 0, OriginalChannels = STBI_default;

		if (!stbi_info_from_memory(Data, DataSize, &Width, &Height, &OriginalChannels))
		{
			LOG_CAT_ERROR(LogImageImporter, "Couldn't parse image header, image path: {}, fail reason: {}", Image.GetPath().generic_string(), stbi_failure_reason());
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
			LOG_CAT_ERROR(LogImageImporter, "Failed to load image: {}, fail reason: {}", Image.GetPath().generic_string(), stbi_failure_reason());
		}

		auto CreateInfo = RHITextureCreateInfo()
			.SetWidth(Width)
			.SetHeight(Height)
			.SetDimension(ERHITextureDimension::T_2D)
			.SetFormat(Image.IsSRGB() ? ERHIFormat::RGBA8_UNorm_SRGB : ERHIFormat::RGBA8_UNorm)
			.SetName(InAsset.GetPath().filename().string())
			.SetInitialData(DataSize, Image.GetRawData().Data);

		return false;
#if 0
		RHI::ImageDesc Desc;
		Desc.Width = Width;
		Desc.Height = Height;
		Desc.Format = RHI::EFormat::RGBA8_UNorm;
		Desc.Type = RHI::EImageType::T_2D;
		Desc.ArrayLayers = 1u;
		Desc.MipLevels = 1u;
		Desc.Subresources.resize(1u);
		Desc.Subresources[0].Width = Width;
		Desc.Subresources[0].Height = Height;
		RHI::FormatAttribute::CalculateFormatBytes(Width, Height, RHI::EFormat::RGBA8_UNorm, Desc.Subresources[0].SliceBytes, Desc.Subresources[0].RowBytes);
		uint32_t Bytes = Width * RHI::FormatAttribute::Attribute(RHI::EFormat::RGBA8_UNorm).BytesPerPixel * Height;
		assert(Bytes == Desc.Subresources[0].SliceBytes);
		Desc.Asset.Data = Pixels;
		Desc.Asset.Size = Bytes;

		return Desc;
#endif
	}
};
