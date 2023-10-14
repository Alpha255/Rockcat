#pragma once

#include "Runtime/Core/ObjectID.h"

struct SceneGraph
{
	DECLARE_OBJECT_ID(Node, uint32_t)

	using MeshID = uint32_t;
	using TransformID = uint32_t;

	class Node
	{
	public:
		enum class ENodeMasks
		{
			None,
			StaticMesh = 1 << 0,
			SkeletalMesh = 1 << 1,
			Light = 1 << 2,
			Camera = 1 << 3
		};

		Node() = default;

		Node(const char8_t* Name, NodeID ParentID = NodeID(), ENodeMasks Masks = ENodeMasks::None)
			: m_Parent(ParentID)
			, m_Masks(Masks)
			, m_Name(Name ? Name : "")
		{
		}

		Node(const Node&) = default;
		Node(Node&&) = default;
		Node& operator=(const Node&) = default;

		bool8_t HasParent() const { return m_Parent.IsValid(); }
		NodeID GetParent() const { return m_Parent; }
		Node& SetParent(NodeID ParentID) { m_Parent = ParentID; return *this; }

		bool8_t HasChild() const { return m_Child.IsValid(); }
		NodeID GetChild() const { return m_Child; }
		Node& SetChild(NodeID ChildID) { m_Child = ChildID; return *this; }

		bool8_t HasSibling() const { return m_Sibling.IsValid(); }
		NodeID GetSibling() const { return m_Sibling; }
		Node& SetSibling(NodeID SiblingID) { m_Sibling = SiblingID; return *this; }

		bool8_t IsAlive() const { return m_Alive; }
		Node& SetAlive(bool8_t Alive) { m_Alive = Alive; return *this; }

		bool8_t IsVisible() const { return m_Visible; }
		Node& SetVisible(bool8_t Visible) { m_Visible = Visible; return *this; }

		bool8_t IsSelected() const { return m_Selected; }
		Node& SetSelected(bool8_t Selected) { m_Selected = Selected; return *this; }

		const char8_t* GetName() const { return m_Name.c_str(); }
		Node& SetName(const char8_t* Name) { m_Name = Name; return *this; }

		ENodeMasks GetMasks() const { return m_Masks; }
		Node& SetMasks(ENodeMasks Masks);

		bool8_t IsStaticMesh() const;
		bool8_t IsSkeletalMesh() const;
		bool8_t IsLight() const;
		bool8_t IsCamera() const;

		uint32_t GetDataIndex() const { return m_DataIndex; }

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(m_Parent),
				CEREAL_NVP(m_Child),
				CEREAL_NVP(m_Sibling),
				CEREAL_NVP(m_Masks),
				CEREAL_NVP(m_Alive),
				CEREAL_NVP(m_Visible),
				CEREAL_NVP(m_Selected),
				CEREAL_NVP(m_Name)
			);
		}
	protected:
		friend class AssimpSceneImporter;
		void SetDataIndex(uint32_t Index) { m_DataIndex = Index; }
	private:
		NodeID m_Parent;
		NodeID m_Child;
		NodeID m_Sibling;

		ENodeMasks m_Masks = ENodeMasks::None;
		uint32_t m_DataIndex = 0u;

		bool8_t m_Alive = true;
		bool8_t m_Visible = true;
		bool8_t m_Selected = false;

		std::string m_Name;
	};
	using NodeList = std::vector<Node>;

	NodeID Root;
	NodeList Nodes;
	NodeIDAllocator IDAllocator;

	size_t GetNodeCount() const { return Nodes.size(); }
	bool8_t IsEmpty() const { return Nodes.empty(); }

	NodeID AddSibling(NodeID Sibling, const char8_t* Name, Node::ENodeMasks Masks = Node::ENodeMasks::None)
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

	NodeID AddChild(NodeID Parent, const char8_t* Name, Node::ENodeMasks Masks = Node::ENodeMasks::None)
	{
		assert(Parent.IsValid() && Parent.GetIndex() < Nodes.size());
		if (Nodes[Parent.GetIndex()].HasChild())
		{
			return AddSibling(Nodes[Parent.GetIndex()].GetChild(), Name);
		}
		else
		{
			auto NextID = AddNode(Parent, Name, Masks);
			Nodes[Parent.GetIndex()].SetChild(NextID);
			return NextID;
		}
	}

	NodeID AddNode(NodeID Parent, const char8_t* Name, Node::ENodeMasks Masks = Node::ENodeMasks::None)
	{
		NodeID NextID = IDAllocator.Allocate();  /// +1
		Nodes.emplace_back(Node(Name, Parent, Masks));
		return NextID;
	}

	const Node* FindNode(const char8_t* NodeName) const
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

	Node& GetNode(const NodeID& ID) 
	{ 
		assert(ID.IsValid() && ID.GetIndex() < Nodes.size());
		return Nodes[ID.GetIndex()]; 
	}

	const Node& GetNode(const NodeID& ID) const
	{
		assert(ID.IsValid() && ID.GetIndex() < Nodes.size());
		return Nodes[ID.GetIndex()];
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Root),
			CEREAL_NVP(Nodes),
			CEREAL_NVP(IDAllocator)
		);
	}
};

ENUM_FLAG_OPERATORS(SceneGraph::Node::ENodeMasks)
