#pragma once

#include "Asset/Asset.h"
#include "RHI/RHITexture.h"

class Texture : public Asset
{
public:
	using Asset::Asset;

	bool IsLinear() const { return m_Linear; }

	const RHITexture* GetRHI() const { assert(m_Texture); return m_Texture.get(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		std::string Path = GetPath().string();

		Ar(
			CEREAL_BASE(Asset),
			cereal::make_nvp("m_Path", Path)
		);
		
		if (Archive::is_loading::value)
		{
			SetPath(Path);
		}
	}
protected:
	friend class AssimpSceneLoader;
	friend class StbTextureLoader;
	friend class DDSTextureLoader;

	void CreateRHI(class RHIDevice& Device, const RHITextureDesc& Desc);

	void SetLinear(bool Linear) { m_Linear = Linear; }

	bool m_Linear = false;
	RHITexturePtr m_Texture;
};

namespace cereal
{
	template<> struct LoadAndConstruct<Texture>
	{
		template<class Archive>
		static void load_and_construct(Archive& Ar, cereal::construct<Texture>& Construct)
		{
			std::string Path;
			Ar(
				cereal::make_nvp("m_Path", Path)
			);
			Construct(Path);
		}
	};
}
