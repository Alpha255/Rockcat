#pragma once

#include "Asset/Asset.h"
#include "RHI/RHITexture.h"

class TextureAsset : public Asset
{
public:
	using Asset::Asset;

	bool IsLinear() const { return m_Linear; }

	const RHITexture* GetRHI() const { assert(m_Texture); return m_Texture.get(); }

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
	friend class StbImageImporter;
	friend class DDSImageImporter;

	void CreateRHI(class RHIDevice& Device, const RHITextureDesc& Desc);

	void SetLinear(bool Linear) { m_Linear = Linear; }

	bool m_Linear = false;
	RHITexturePtr m_Texture;
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
