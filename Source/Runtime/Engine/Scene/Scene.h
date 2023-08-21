#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Asset/SceneAsset.h"

class Scene : public SceneAsset
{
public:
	using SceneAsset::SceneAsset;

	const SceneData& GetSceneData() const { return m_Data; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneAsset)
		);
	}
protected:
	friend class SceneBuilder;
private:
	SceneData m_Data;
};

