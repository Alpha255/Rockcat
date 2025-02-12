#pragma once

#include "Engine/Asset/SceneAsset.h"
#include "Engine/Tickable.h"

class Scene : public SceneAsset, public ITickable
{
public:
	using SceneAsset::SceneAsset;

	~Scene() { Save<Scene>(IsDirty()); }

	inline const std::vector<SceneGraph::NodeID>& GetAddNodes() const { return m_NodeCollection.AddNodes; }
	inline const std::vector<SceneGraph::Node>& GetRemovedNodes() const { return m_NodeCollection.RemovedNodes; }

	inline size_t GetNumNodes() const { return GetGraph().Nodes.size(); }
	inline uint32_t GetNumPrimitives() const { return GetGraph().NumPrimitives; }
	inline const SceneGraph::Node* GetNode(const SceneGraph::NodeID& ID) const { return GetGraph().GetNode(ID); }

	void Tick(float ElapsedSeconds) override final;
protected:
	struct NodeCollection
	{
		std::vector<SceneGraph::NodeID> AddNodes;
		std::vector<SceneGraph::Node> RemovedNodes;

		inline bool HasDirtyNodes() const
		{
			return !AddNodes.empty() || !RemovedNodes.empty();
		}

		void ClearDirtyNodes()
		{
			AddNodes.clear();
			RemovedNodes.clear();
		}
	};

	inline void SetDirty(bool Dirty) { m_Dirty = Dirty; }
	inline bool IsDirty() const { return m_Dirty; }

	void UpdateNodeCollection();
private:
	bool m_Dirty = false;
	NodeCollection m_NodeCollection;
};

