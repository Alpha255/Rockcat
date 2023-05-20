#pragma once

#include "Runtime/Core/ObjectID.h"

class DirectedAcyclicGraph
{
public:
	using NodeIDAllocator = ObjectIDAllocator<class Node, uint16_t>;
	using NodeID = NodeIDAllocator::TID;

	using EdgeIDAllocator = ObjectIDAllocator<class Edge, uint16_t>;
	using EdgeID = EdgeIDAllocator::TID;

	DECLARE_SMART_PTR(Node)
	DECLARE_SMART_PTR(Edge)

	class Node
	{
	public:
		Node(NodeID ID)
			: m_ID(ID)
		{
		}

		void ConnectInputEdge(const EdgeID& ID)
		{
			assert(std::find(m_InputEdges.begin(), m_InputEdges.end(), ID) == m_InputEdges.end());
			m_InputEdges.push_back(ID);
		}

		void ConnectOutputEdge(const EdgeID& ID)
		{
			assert(std::find(m_OutputEdges.begin(), m_OutputEdges.end(), ID) == m_OutputEdges.end());
			m_OutputEdges.push_back(ID);
		}

		void RemoveEdge(const EdgeID& ID)
		{
			auto EdgeIt = std::find(m_InputEdges.begin(), m_InputEdges.end(), ID);
			if (EdgeIt != m_InputEdges.end())
			{
				m_InputEdges.erase(EdgeIt);
			}

			EdgeIt = std::find(m_OutputEdges.begin(), m_OutputEdges.end(), ID);
			if (EdgeIt != m_OutputEdges.end())
			{
				m_OutputEdges.erase(EdgeIt);
			}
		}

		const std::vector<EdgeID>& GetInputEdges() const
		{
			return m_InputEdges;
		}

		const std::vector<EdgeID>& GetOutputEdges() const
		{
			return m_OutputEdges;
		}

		NodeID GetID() const
		{
			return m_ID;
		}

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(m_InputEdges),
				CEREAL_NVP(m_OutputEdges),
				CEREAL_NVP(m_ID)
			);
		}
	protected:
	private:
		std::vector<EdgeID> m_InputEdges;
		std::vector<EdgeID> m_OutputEdges;
		NodeID m_ID;
	};

	class Edge
	{
	public:
		Edge(EdgeID ID, const NodeID& SrcNode, const NodeID& DstNode)
			: m_ID(ID)
			, m_SrcNode(SrcNode)
			, m_DstNode(DstNode)
		{
		}

		NodeID GetSourceNode() const
		{
			return m_SrcNode;
		}

		NodeID GetDestinationNode() const
		{
			return m_DstNode;
		}

		EdgeID GetID() const
		{
			return m_ID;
		}

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(m_SrcNode),
				CEREAL_NVP(m_DstNode),
				CEREAL_NVP(m_ID)
			);
		}
	protected:
	private:
		NodeID m_SrcNode;
		NodeID m_DstNode;
		EdgeID m_ID;
	};

	bool8_t IsNodeExists(const NodeID& ID) const
	{
		return m_Nodes.find(ID.GetIndex()) != m_Nodes.end();
	}

	bool8_t IsEdgeExists(const EdgeID& ID) const
	{
		return m_Edges.find(ID.GetIndex()) != m_Edges.end();
	}

	const Node* FindNode(const NodeID& ID) const
	{
		if (IsNodeExists(ID))
		{
			return m_Nodes.at(ID.GetIndex()).get();
		}

		return nullptr;
	}

	const Edge* FindEdge(const EdgeID& ID) const
	{
		if (IsEdgeExists(ID))
		{
			return m_Edges.at(ID.GetIndex()).get();
		}

		return nullptr;
	}

	NodeID AddNode()
	{
		auto NewNode = std::make_shared<Node>(m_NodeIDAllocator.Allocate());
		m_Nodes.insert(std::make_pair(NewNode->GetID().GetIndex(), NewNode));
		return NewNode->GetID();
	}

	void AddNode(const NodeSharedPtr& NewNode)
	{
		m_Nodes.insert(std::make_pair(NewNode->GetID().GetIndex(), NewNode));
	}

	std::vector<EdgeID> RemoveNode(const NodeID& ID)
	{
		std::vector<EdgeID> EdgesToRemove;

		if (auto Node = FindNode(ID))
		{
			EdgesToRemove.insert(EdgesToRemove.end(), Node->GetInputEdges().begin(), Node->GetInputEdges().end());
			EdgesToRemove.insert(EdgesToRemove.end(), Node->GetOutputEdges().begin(), Node->GetOutputEdges().end());

			for (auto& EdgeID : EdgesToRemove)
			{
				m_Edges.at(EdgeID.GetIndex()).reset();
			}

			m_Nodes.at(ID.GetIndex()).reset();
		}

		return EdgesToRemove;
	}

	EdgeID AddEdge(const NodeID& SrcNode, const NodeID& DstNode)
	{
		if (IsNodeExists(SrcNode) && IsNodeExists(DstNode))
		{
			auto ID = m_EdgeIDAllocator.Allocate();
			m_Nodes.at(SrcNode.GetIndex())->ConnectOutputEdge(ID);
			m_Nodes.at(DstNode.GetIndex())->ConnectInputEdge(ID);
			m_Edges.insert(std::make_pair(ID.GetIndex(), std::make_shared<Edge>(ID, SrcNode, DstNode)));

			return ID;
		}

		return EdgeID();
	}

	void RemoveEdge(const EdgeID& ID)
	{
		if (auto Edge = FindEdge(ID))
		{
			m_Nodes.at(Edge->GetSourceNode().GetIndex())->RemoveEdge(ID);
			m_Nodes.at(Edge->GetDestinationNode().GetIndex())->RemoveEdge(ID);

			m_Edges.at(ID.GetIndex()).reset();
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Nodes),
			CEREAL_NVP(m_Edges),
			CEREAL_NVP(m_NodeIDAllocator),
			CEREAL_NVP(m_EdgeIDAllocator)
		);
	}
protected:
private:
	std::unordered_map<NodeID::IndexType, NodeSharedPtr> m_Nodes;
	std::unordered_map<EdgeID::IndexType, EdgeSharedPtr> m_Edges;

	NodeIDAllocator m_NodeIDAllocator;
	EdgeIDAllocator m_EdgeIDAllocator;
};

