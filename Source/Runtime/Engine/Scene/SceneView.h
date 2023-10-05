#pragma once

#include "Runtime/Engine/Scene/Scene.h"

struct DepthFirst {};
struct BreadthFirst {};

template<class Policy = DepthFirst>
class SceneView
{
public:
	using SceneNodeIterator = SceneGraph::NodeList::const_iterator;

	SceneView(const Scene& TargetScene)
		: SceneView(TargetScene.GetSceneGraph())
	{
	}

	void Next() { Next(Policy()); }

	SceneNodeIterator& Get() { return m_Iterator; }
private:
	SceneView(const SceneGraph& Graph)
		: m_Graph(Graph)
		, m_Iterator(std::next(Graph.Nodes.cbegin(), Graph.Root.GetIndex()))
	{
	}

	void MoveTo(const SceneGraph::NodeID& Diff) { MoveTo(Diff.GetIndex()); }
	void MoveTo(const SceneGraph::NodeID::IndexType Diff) { m_Iterator = std::next(m_Graph.Nodes.cbegin(), Diff); }

	void Next(DepthFirst)
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
	}

	void Next(BreadthFirst)
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
	}

	const SceneGraph& m_Graph;
	SceneNodeIterator m_Iterator;
	std::stack<SceneGraph::NodeID::IndexType> m_Pending;
};