#pragma once

#include "Runtime/Core/ObjectID.h"

NAMESPACE_START(Gear)

class Scene
{
public:
	DECLARE_OBJECT_ID(Node, uint32_t)

	class Node
	{
	public:
		Node(const char8_t* Name, NodeID ParentID = NodeID())
			: m_Name(Name)
		{
		}

		bool8_t HasParent() const { return m_Parent.IsValid(); }
		NodeID GetParent() const { return m_Parent; }
		void SetParent(NodeID ParentID) { m_Parent = ParentID; }

		bool8_t HasChild() const { return m_Child.IsValid(); }
		NodeID GetChild() const { return m_Child; }
		void SetChild(NodeID ChildID) { m_Child = ChildID; }

		bool8_t HasSibling() const { return m_Sibling.IsValid(); }
		NodeID GetSibling() const { return m_Sibling; }
		void SetSibling(NodeID SiblingID) { m_Sibling = SiblingID; }

		bool8_t IsAlive() const { return m_Alive; }
		void SetAlive(bool8_t Alive) { m_Alive = Alive; }

		bool8_t IsVisible() const { return m_Visible; }
		void SetVisible(bool8_t Visible) { m_Visible = Visible; }

		const char8_t* GetName() const { return m_Name.data(); }
		void SetName(const char8_t* Name) { m_Name = Name; }

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(m_Parent),
				CEREAL_NVP(m_Child),
				CEREAL_NVP(m_Sibling),
				CEREAL_NVP(m_Alive),
				CEREAL_NVP(m_Visible),
				CEREAL_NVP(m_Name)
			);
		}
	protected:
	private:
		NodeID m_Parent;
		NodeID m_Child;
		NodeID m_Sibling;

		bool8_t m_Alive = true;
		bool8_t m_Visible = true;

		std::string_view m_Name;
	};

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Graph),
			CEREAL_NVP(m_Data)
		);
	}
protected:
	struct SceneGraph
	{
		NodeID Root;
		std::vector<Node> Nodes;
		NodeIDAllocator IDAllocator;

		size_t GetNodeCount() const { return Nodes.size(); }

		NodeID AddSibling(NodeID Sibling, const char8_t* Name)
		{
			assert(Sibling.IsValid() && Sibling.GetIndex() < Nodes.size());
			NodeID::IndexType SiblingIndex = Sibling.GetIndex();
			while (Nodes[SiblingIndex].HasSibling())
			{
				SiblingIndex = Nodes[SiblingIndex].GetSibling().GetIndex();
			}

			NodeID SiblingNode = AddNode(Nodes[SiblingIndex].GetParent(), Name);
			Nodes[Sibling.GetIndex()].SetSibling(SiblingNode);
			return SiblingNode;
		}

		NodeID AddChild(NodeID Parent, const char8_t* Name)
		{
			assert(Parent.IsValid() && Parent.GetIndex() < Nodes.size());
			Node& ParentNode = Nodes[Parent.GetIndex()];
			if (ParentNode.HasChild())
			{
				return AddSibling(ParentNode.GetChild(), Name);
			}
			else
			{
				return AddNode(Parent, Name);
			}

			return NodeID();
		}

		NodeID AddNode(NodeID Parent, const char8_t* Name)
		{
			NodeID NextID = IDAllocator.Allocate();
			Nodes.emplace_back(Node(Name, Parent));
			return NextID;
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

	const SceneGraph& GetGraph() const { return m_Graph; }

	struct SceneData
	{
		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
			);
		}
	};
private:
	SceneGraph m_Graph;
	SceneData m_Data;
};

NAMESPACE_END(Gear)