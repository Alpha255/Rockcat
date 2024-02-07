#pragma once

#include "Engine/Asset/SceneAsset.h"

class Scene : public SceneAsset
{
public:
	using SceneAsset::SceneAsset;

	~Scene() { Save<Scene>(true); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneAsset)
		);
	}

	bool IsDirty() const { return m_Dirty; }
protected:
private:
	bool m_Dirty = false;
};

