#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/RHI/RHIDeclarations.h"

struct ImageAsset : public Asset
{
public:
	using Asset::Asset;

	bool IsSRGB() const { return SRGB; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		std::string Path = m_Path.generic_string();
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
	RHIImagePtr Image;
};

namespace cereal
{
	template<> struct LoadAndConstruct<ImageAsset>
	{
		template<class Archive>
		static void load_and_construct(Archive& Ar, cereal::construct<ImageAsset>& Construct)
		{
			std::string Path;
			Ar(
				cereal::make_nvp("m_Path", Path)
			);
			Construct(Path);
		}
	};
}
