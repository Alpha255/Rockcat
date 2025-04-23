#pragma once

#include "Asset/SceneAsset.h"
#include "Core/Tickable.h"

class Scene : public SceneAsset, public ITickable
{
public:
	// #TODO: Save scene data into custom file format, otherwise, the scene data will be lost after the application is closed
	using SceneAsset::SceneAsset;

	~Scene();

	inline const std::vector<SceneGraph::NodeID>& GetAddNodes() const { return m_NodeCollection.AddNodes; }
	inline const std::vector<SceneGraph::Node>& GetRemovedNodes() const { return m_NodeCollection.RemovedNodes; }

	inline size_t GetNumNodes() const { return GetGraph().Nodes.size(); }
	inline uint32_t GetNumPrimitives() const { return GetGraph().NumPrimitives; }
	inline const SceneGraph::Node* GetNode(const SceneGraph::NodeID& ID) const { return GetGraph().GetNode(ID); }

	void Tick(float ElapsedSeconds) override final;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneAsset)
		);
	}
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

