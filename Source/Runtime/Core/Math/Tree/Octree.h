#pragma once

#include "Runtime/Core/Math/Matrix.h"

template<class T>
class Octree
{
public:
	using NodeIndex = uint32_t;
	static constexpr NullIndex = std::numeric_limits<NodeIndex>::max();

	class ElementID
	{
	public:
		template<class T>
		friend class Octree;

		ElementID() = default;

		bool8_t IsValid() const { return m_NodeIndex != NullIndex; }
		NodeIndex GetNodeIndex() const { return m_NodeIndex; }
	private:
		ElementID(NodeIndex InNodeIndex, uint32_t InElementIndex)
			: m_NodeIndex(InNodeIndex)
			, m_ElementIndex(InElementIndex)
		{
		}

		NodeIndex m_NodeIndex = NullIndex;
		uint32_t m_ElementIndex = std::numeric_limits<uint32_t>::max();
	};

	size_t GetNumNodes() const { return m_Nodes.size(); }

	void AddElement(const T& Element)
	{
		ElementList TempElements;
	}

	void RemoveElement(ElementID InElementID)
	{
	}
private:
	using ElementList = std::vector<T>;
	struct Node
	{
		NodeIndex ChildNodes = NullIndex;
		uint32_t NumElements = 0u;

		bool8_t IsLeaf() const { return ChildNodes == NullIndex; }
	};

	std::vector<Node> m_Nodes;
	std::vector<ElementList> m_Elements;
	std::vector<NodeIndex> m_ParentLinks;
};
