#pragma once

#include "Core/ObjectID.h"

class DirectedAcyclicGraph
{
public:
	using NodeID = ObjectID<class Node>;
	using EdgeID = ObjectID<class Edge>;

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

		const std::vector<EdgeID>& GetInputEdges() const { return m_InputEdges; }
		const std::vector<EdgeID>& GetOutputEdges() const { return m_OutputEdges; }

		NodeID GetID() const { return m_ID; }

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

		NodeID GetSourceNode() const { return m_SrcNode; }
		NodeID GetDestinationNode() const { return m_DstNode; }

		EdgeID GetID() const { return m_ID; }

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

	bool IsNodeExists(const NodeID& ID) const { return m_Nodes.find(ID) != m_Nodes.end(); }
	bool IsEdgeExists(const EdgeID& ID) const { return m_Edges.find(ID) != m_Edges.end(); }

	const Node* GetNode(const NodeID& ID) const
	{
		auto It = m_Nodes.find(ID);
		if (It != m_Nodes.end())
		{
			return &It->second;
		}

		return nullptr;
	}

	const Edge* GetEdge(const EdgeID& ID) const
	{
		auto It = m_Edges.find(ID);
		if (It != m_Edges.end())
		{
			return &It->second;
		}

		return nullptr;
	}

	NodeID AddNode()
	{
		auto ID = NodeID(m_NextNodeID++);
		m_Nodes.insert(std::make_pair(ID, Node(ID)));
		return ID;
	}

	std::vector<EdgeID> RemoveNode(const NodeID& ID)
	{
		std::vector<EdgeID> EdgesToRemove;

		if (auto Node = GetNode(ID))
		{
			EdgesToRemove.insert(EdgesToRemove.end(), Node->GetInputEdges().begin(), Node->GetInputEdges().end());
			EdgesToRemove.insert(EdgesToRemove.end(), Node->GetOutputEdges().begin(), Node->GetOutputEdges().end());

			for (auto& EdgeID : EdgesToRemove)
			{
				m_Edges.erase(EdgeID);
			}

			m_Nodes.erase(ID);
		}

		return EdgesToRemove;
	}

	EdgeID AddEdge(const NodeID& SrcNode, const NodeID& DstNode)
	{
		if (IsNodeExists(SrcNode) && IsNodeExists(DstNode))
		{
			auto ID = EdgeID(m_NextEdgeID++);
			m_Nodes.at(SrcNode).ConnectOutputEdge(ID);
			m_Nodes.at(DstNode).ConnectInputEdge(ID);
			m_Edges.insert(std::make_pair(ID, Edge(ID, SrcNode, DstNode)));

			return ID;
		}

		return EdgeID();
	}

	void RemoveEdge(const EdgeID& ID)
	{
		if (auto Edge = GetEdge(ID))
		{
			m_Nodes.at(Edge->GetSourceNode()).RemoveEdge(ID);
			m_Nodes.at(Edge->GetDestinationNode()).RemoveEdge(ID);

			m_Edges.erase(ID);
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Nodes),
			CEREAL_NVP(m_Edges)
		);
	}
protected:
private:
	NodeID::IndexType m_NextNodeID = 0u;
	EdgeID::IndexType m_NextEdgeID = 0u;

	std::unordered_map<NodeID, Node> m_Nodes;
	std::unordered_map<EdgeID, Edge> m_Edges;
};

