#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/RHI/RHIDeclarations.h"

struct TextureAsset : public Asset
{
public:
	using Asset::Asset;

	bool IsSRGB() const { return SRGB; }

	const RHITexture* GetRHI() const { return Texture; }
	void CreateRHI(class RHIDevice& Device, const struct RHITextureCreateInfo& CreateInfo);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		std::string Path = m_Path.string();
		Ar(
			cereal::make_nvp("m_Path", Path)
		);

		if (Archive::is_loading::value)
		{
			m_Path = Path;
		}
	}
protected:
	friend class AssimpSceneImporter;

	void SetUseSRGB(bool InSRGB) { SRGB = InSRGB; }

	bool SRGB = false;
	const RHITexture* Texture = nullptr;
};

namespace cereal
{
	template<> struct LoadAndConstruct<TextureAsset>
	{
		template<class Archive>
		static void load_and_construct(Archive& Ar, cereal::construct<TextureAsset>& Construct)
		{
			std::string Path;
			Ar(
				cereal::make_nvp("m_Path", Path)
			);
			Construct(Path);
		}
	};
}
