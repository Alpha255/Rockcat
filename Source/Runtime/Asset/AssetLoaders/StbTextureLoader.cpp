#include "Asset/AssetLoaders/StbTextureLoader.h"
#include "Asset/Texture.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIDevice.h"
#include "Services/SpdLogService.h"

#pragma warning(disable:4244)
#include <Submodules/stb/stb_image.h>
#pragma warning(default:4244)

StbTextureLoader::StbTextureLoader()
	: AssetLoader({
		".jpeg",
		".jpg",
		".png",
		".bmp",
		".tga",
		".psd",
		".hdr"
	})
{
	LOG_INFO_CAT(LogAsset, "Create stb image loader, use stb_image @2.3");
}

std::shared_ptr<Asset> StbTextureLoader::CreateAssetImpl(const std::filesystem::path& Path)
{
	return std::make_shared<Texture>(Path);
}

bool StbTextureLoader::Load(Asset& InAsset, const AssetType& Type)
{
	auto& StbImage = Cast<Texture>(InAsset);
	auto AssetData = StbImage.LoadData();
	auto const DataSize = static_cast<int32_t>(AssetData->Size);
	auto Data = reinterpret_cast<const stbi_uc*>(AssetData->Data.get());

	int32_t Width = 0, Height = 0, Channels = 0, OriginalChannels = STBI_default;

	if (!stbi_info_from_memory(Data, DataSize, &Width, &Height, &OriginalChannels))
	{
		LOG_ERROR_CAT(LogAsset, "Couldn't parse image header, image path: {}, fail reason: {}", StbImage.GetPath().string(), stbi_failure_reason());
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
		LOG_ERROR_CAT(LogAsset, "Failed to load image: {}, fail reason: {}", StbImage.GetPath().string(), stbi_failure_reason());
		return false;
	}

	RHITextureDesc Desc;
	Desc.SetWidth(Width)
		.SetHeight(Height)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(StbImage.IsLinear() ? ERHIFormat::RGBA8_UNorm : ERHIFormat::RGBA8_UNorm_SRGB)
		.SetUsages(ERHIBufferUsageFlags::ShaderResource)
		.SetName(InAsset.GetPath().filename().string())
		.SetPermanentState(ERHIResourceState::ShaderResource)
		.SetInitialData(DataBlock(Channels * Width * Height, Bitmap));

	//StbImage.CreateRHI(RenderService::Get().GetBackend().GetDevice(), Desc);		

	return true;
}
