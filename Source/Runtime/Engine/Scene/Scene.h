#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Asset/SceneAsset.h"

class Scene : public SceneAsset
{
public:
	using SceneAsset::SceneAsset;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneAsset)
		);
	}
protected:
private:
};

