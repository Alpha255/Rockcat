#pragma once

#include "Core/ObjectID.h"
#include "Scene/Components/Components.h"

struct SceneGraph
{
	using NodeID = ObjectID<class Node>;

	class Node
	{
	public:
		enum class ENodeMasks
		{
			None,
			Primitive = 1 << 0,
			Light = 1 << 1,
			Camera = 1 << 2
		};

		Node() = default;

		Node(const char* Name, NodeID ID, NodeID ParentID = NodeID(), ENodeMasks Masks = ENodeMasks::None)
			: m_ID(ID)
			, m_Parent(ParentID)
			, m_Masks(Masks)
			, m_Name(Name ? Name : "")
		{
		}

		Node(const Node&) = default;
		Node(Node&&) = default;
		Node& operator=(const Node&) = default;

		inline NodeID GetID() const { return m_ID; }

		inline bool HasParent() const { return m_Parent.IsValid(); }
		inline NodeID GetParent() const { return m_Parent; }
		inline Node& SetParent(NodeID ParentID) { m_Parent = ParentID; return *this; }

		inline bool HasChild() const { return m_Child.IsValid(); }
		inline NodeID GetChild() const { return m_Child; }
		inline Node& SetChild(NodeID ChildID) { m_Child = ChildID; return *this; }

		inline bool HasSibling() const { return m_Sibling.IsValid(); }
		inline NodeID GetSibling() const { return m_Sibling; }
		inline Node& SetSibling(NodeID SiblingID) { m_Sibling = SiblingID; return *this; }

		inline bool IsVisible() const { return m_Visible; }
		inline Node& SetVisible(bool Visible) { m_Visible = Visible; return *this; }

		inline bool IsSelected() const { return m_Selected; }
		inline Node& SetSelected(bool Selected) { m_Selected = Selected; return *this; }

		inline const char* GetName() const { return m_Name.c_str(); }
		inline Node& SetName(const char* Name) { m_Name = Name; return *this; }

		inline ENodeMasks GetMasks() const { return m_Masks; }

		inline uint32_t GetDataIndex() const { return m_DataIndex; }

		bool IsPrimitive() const;
		bool IsLight() const;
		bool IsCamera() const;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(m_ID),
				CEREAL_NVP(m_Parent),
				CEREAL_NVP(m_Child),
				CEREAL_NVP(m_Sibling),
				CEREAL_NVP(m_Masks),
				CEREAL_NVP(m_Visible),
				CEREAL_NVP(m_Selected),
				CEREAL_NVP(m_Name)
			);
		}
	protected:
		friend class AssimpSceneImporter;
		friend struct SceneGraph;
		friend class SceneBuilder;

		inline void SetDataIndex(uint32_t Index) { m_DataIndex = Index; }
		inline void SetID(NodeID ID) { m_ID = ID; }

		inline bool IsAlive() const { return m_Alive; }
		inline Node& SetAlive(bool Alive) { m_Alive = Alive; return *this; }
	private:
		NodeID m_ID;
		NodeID m_Parent;
		NodeID m_Child;
		NodeID m_Sibling;

		ENodeMasks m_Masks = ENodeMasks::None;
		uint32_t m_DataIndex = 0u;

		bool m_Alive = true;
		bool m_Visible = true;
		bool m_Selected = false;

		std::string m_Name;

		//std::vector<ComponentBase*> m_Components;
	};
	using NodeList = std::vector<Node>;

	uint32_t NumPrimitives = 0u;
	NodeID Root;
	NodeList Nodes;

	NodeID AddSibling(NodeID Sibling, const char* Name, Node::ENodeMasks Masks = Node::ENodeMasks::None)
	{
		assert(Sibling.IsValid() && Sibling.GetIndex() < Nodes.size());
		NodeID::IndexType SiblingIndex = Sibling.GetIndex();
		while (Nodes[SiblingIndex].HasSibling())
		{
			SiblingIndex = Nodes[SiblingIndex].GetSibling().GetIndex();
		}

		NodeID SiblingNode = AddNode(Nodes[SiblingIndex].GetParent(), Name, Masks);
		Nodes[Sibling.GetIndex()].SetSibling(SiblingNode);
		return SiblingNode;
	}

	NodeID AddChild(NodeID Parent, const char* Name, Node::ENodeMasks Masks = Node::ENodeMasks::None)
	{
		assert(Parent.IsValid() && Parent.GetIndex() < Nodes.size());
		if (Nodes[Parent.GetIndex()].HasChild())
		{
			return AddSibling(Nodes[Parent.GetIndex()].GetChild(), Name, Masks);
		}
		else
		{
			auto NextID = AddNode(Parent, Name, Masks);
			Nodes[Parent.GetIndex()].SetChild(NextID);
			return NextID;
		}
	}

	NodeID AddNode(NodeID Parent, const char* Name, Node::ENodeMasks Masks = Node::ENodeMasks::None)
	{
		NodeID ID = NodeID(static_cast<NodeID::IndexType>(Nodes.size()));
		if (Nodes.emplace_back(Node(Name, ID, Parent, Masks)).IsPrimitive())
		{
			++NumPrimitives;
		}
		return ID;
	}

	Node RemoveNode(NodeID ID);
	void PuregeDeadNodes();

	const Node* GetNodeByName(const char* NodeName) const
	{
		for (auto& Node : Nodes)
		{
			if (strcmp(Node.GetName(), NodeName) == 0)
			{
				return &Node;
			}
		}
		return nullptr;
	}

	const Node* GetNode(const NodeID& ID) const
	{ 
		assert(ID.IsValid() && ID.GetIndex() < Nodes.size());

		if (Nodes[ID.GetIndex()].IsAlive())
		{
			return &Nodes[ID.GetIndex()];
		}

		return nullptr; 
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Root),
			CEREAL_NVP(Nodes)
		);
	}
};

ENUM_FLAG_OPERATORS(SceneGraph::Node::ENodeMasks)
