#pragma once

#include "Engine/Scene/Scene.h"

struct DepthFirst {};
struct BreadthFirst {};

using SceneNodeIterator = SceneGraph::NodeList::const_iterator;

template<class Policy = DepthFirst>
class SceneVisitor
{
public:
	SceneVisitor(const Scene& InScene)
		: SceneVisitor(InScene.GetGraph())
	{
	}

	SceneNodeIterator Next() { return Next(Policy()); }
	SceneNodeIterator Get() { return m_Iterator; }
private:
	SceneVisitor(const SceneGraph& Graph)
		: m_Graph(Graph)
		, m_Iterator(std::next(Graph.Nodes.cbegin(), Graph.Root.GetIndex()))
	{
	}

	void MoveTo(const SceneGraph::NodeID& Diff) { MoveTo(Diff.GetIndex()); }
	void MoveTo(const SceneGraph::NodeID::IndexType Diff) { m_Iterator = std::next(m_Graph.Nodes.cbegin(), Diff); }

	SceneNodeIterator Next(DepthFirst)
	{
		if (m_Iterator->HasSibling())
		{
			m_Pending.push(m_Iterator->GetSibling().GetIndex());
		}

		if (m_Iterator->HasChild())
		{
			MoveTo(m_Iterator->GetChild());
		}
		else if (!m_Pending.empty())
		{
			MoveTo(m_Pending.top());
			m_Pending.pop();
		}
		else
		{
			m_Iterator = SceneNodeIterator();
		}

		return m_Iterator;
	}

	SceneNodeIterator Next(BreadthFirst)
	{
		if (m_Iterator->HasChild())
		{
			m_Pending.push(m_Iterator->GetChild().GetIndex());
		}

		if (m_Iterator->HasSibling())
		{
			MoveTo(m_Iterator->GetSibling());
		}
		else if (!m_Pending.empty())
		{
			MoveTo(m_Pending.top());
			m_Pending.pop();
		}
		else
		{
			m_Iterator = SceneNodeIterator();
		}

		return m_Iterator;
	}

	const SceneGraph& m_Graph;
	SceneNodeIterator m_Iterator;
	std::stack<SceneGraph::NodeID::IndexType> m_Pending;
};