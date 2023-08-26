#pragma once

#include "Runtime/Engine/Asset/Asset.h"

struct ImageAsset : public Asset
{
public:
	using Asset::Asset;

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
