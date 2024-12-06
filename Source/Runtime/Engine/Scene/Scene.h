#pragma once

#include "Engine/Asset/SceneAsset.h"

class Scene : public SceneAsset
{
public:
	using SceneAsset::SceneAsset;

	~Scene() 
	{
		Save<Scene>(IsDirty()); 
	}

	void Update();

	const std::set<const SceneGraph::Node*>& GetVisibleNodes() const { return m_VisibleNodes; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneAsset)
		);
	}

	inline bool IsDirty() const { return m_Dirty; }
protected:
	inline void SetDirty(bool Dirty) { m_Dirty = Dirty; }
private:
	bool m_Dirty = true;
	std::set<const SceneGraph::Node*> m_VisibleNodes;
	std::vector<const SceneGraph::Node*> m_AddNodes;
	std::vector<const SceneGraph::Node*> m_RemoveOrHiddenNodes;
	std::vector<const SceneGraph::Node*> m_DirtyNodes;
};

