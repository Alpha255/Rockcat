#pragma once

#include "Runtime/Engine/Asset/ImageAsset.h"
#pragma warning(disable:4244)
#include <Submodules/stb/stb_image.h>
#pragma warning(default:4244)

class StbImageImporter : public IAssetImporter
{
public:
	StbImageImporter()
		: IAssetImporter({".jpeg", ".jpg", ".png", ".bmp", ".tga", ".psd", ".hdr"})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<ImageAsset>(AssetPath); }

	bool8_t Reimport(Asset& InAsset) override final
	{
		auto& Image = Cast<ImageAsset>(InAsset);
#if 0
		assert(Data && Size);

		int32_t Width = 0u;
		int32_t Height = 0u;
		int32_t Channels = 0u;
		auto Pixels = stbi_load_from_memory(Data, static_cast<int32_t>(Size), &Width, &Height, &Channels, STBI_rgb_alpha);
		if (!Pixels)
		{
			LOG_ERROR("AssetLoader: Failed to load texture: \"{}\"", stbi_failure_reason());
			assert(0);
		}

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

#if 0
		stbi_image_free(pixels);
#endif
		return Desc;
#endif
		return false;
	}
};
