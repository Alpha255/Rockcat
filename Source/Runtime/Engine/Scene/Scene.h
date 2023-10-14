#pragma once

#include "Runtime/Engine/Asset/SceneAsset.h"

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

	bool8_t IsDirty() const { return m_Dirty; }
protected:
private:
	bool8_t m_Dirty = false;
};

