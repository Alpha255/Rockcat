#pragma once

#include "Runtime/Engine/Asset/Asset.h"

struct ImageAsset : public Asset
{
public:
	using Asset::Asset;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Path)
		);
	}
};
