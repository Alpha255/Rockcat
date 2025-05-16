#pragma once

#include "Scene/Scene.h"

struct DepthFirst {};
struct BreadthFirst {};

using EntityIterator = SceneGraph::EntityList::const_iterator;

template<class Policy = DepthFirst>
class SceneVisitor
{
public:
	SceneVisitor(const Scene& InScene)
		: m_Scene(InScene)
		, m_Iterator(std::next(InScene.GetAllEntities().cbegin(), InScene.GetRoot().GetIndex()))
	{
	}

	EntityIterator Next() { return Next(Policy()); }
private:
	inline void MoveTo(const EntityID& Diff) { MoveTo(Diff.GetIndex()); }
	inline void MoveTo(const EntityID::IndexType Diff) { m_Iterator = std::next(m_Scene.GetAllEntities().cbegin(), Diff); }

	EntityIterator Next(DepthFirst)
	{
		if (m_Iterator->HasSibling())
		{
			m_Stack.push(m_Iterator->GetSibling().GetIndex());
		}

		if (m_Iterator->HasChild())
		{
			MoveTo(m_Iterator->GetChild());
		}
		else if (!m_Stack.empty())
		{
			MoveTo(m_Stack.top());
			m_Stack.pop();
		}
		else
		{
			m_Iterator = EntityIterator();
		}

		return m_Iterator;
	}

	EntityIterator Next(BreadthFirst)
	{
		if (m_Iterator->HasChild())
		{
			m_Stack.push(m_Iterator->GetChild().GetIndex());
		}

		if (m_Iterator->HasSibling())
		{
			MoveTo(m_Iterator->GetSibling());
		}
		else if (!m_Stack.empty())
		{
			MoveTo(m_Stack.top());
			m_Stack.pop();
		}
		else
		{
			m_Iterator = EntityIterator();
		}

		return m_Iterator;
	}

	const Scene& m_Scene;
	EntityIterator m_Iterator;
	std::stack<EntityID::IndexType> m_Stack;
};