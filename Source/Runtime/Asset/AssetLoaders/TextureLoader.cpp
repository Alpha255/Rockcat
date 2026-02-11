#include "Asset/AssetLoaders/TextureLoader.h"
#include "Asset/Texture.h"
#include "Services/SpdLogService.h"

#pragma warning(disable:4244)
#include <Submodules/stb/stb_image.h>
#pragma warning(default:4244)

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
	Image.LoadData();

	return IsDDSTexture(Target.GetExtension()) ? LoadDDS(Image) : LoadStb(Image);

}

bool TextureLoader::LoadStb(Texture& Image)
{
	return false;
}

bool TextureLoader::LoadDDS(Texture& Image)
{
	return false;
}