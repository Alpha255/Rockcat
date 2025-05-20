#pragma once

#include "Core/ObjectID.h"
#include "Scene/Components/ComponentBase.h"

using EntityID = ObjectID<class Entity>;

class Entity
{
public:
	Entity() = default;

	Entity(const char* Name, EntityID ID, EntityID ParentID = EntityID())
		: m_ID(ID)
		, m_Parent(ParentID)
		, m_Name(Name ? Name : "")
	{
	}

	Entity(const Entity&) = default;
	Entity(Entity&&) = default;
	Entity& operator=(const Entity&) = default;

	inline EntityID GetID() const { return m_ID; }

	inline bool HasParent() const { return m_Parent.IsValid(); }
	inline EntityID GetParent() const { return m_Parent; }
	inline Entity& SetParent(EntityID ParentID) { m_Parent = ParentID; return *this; }

	inline bool HasChild() const { return m_Child.IsValid(); }
	inline EntityID GetChild() const { return m_Child; }
	inline Entity& SetChild(EntityID ChildID) { m_Child = ChildID; return *this; }

	inline bool HasSibling() const { return m_Sibling.IsValid(); }
	inline EntityID GetSibling() const { return m_Sibling; }
	inline Entity& SetSibling(EntityID SiblingID) { m_Sibling = SiblingID; return *this; }

	inline bool IsVisible() const { return m_Visible; }
	inline Entity& SetVisible(bool Visible) { m_Visible = Visible; return *this; }

	inline bool IsSelected() const { return m_Selected; }
	inline Entity& SetSelected(bool Selected) { m_Selected = Selected; return *this; }

	inline const char* GetName() const { return m_Name.c_str(); }
	inline Entity& SetName(const char* Name) { m_Name = Name; return *this; }

	inline const std::unordered_set<ComponentBase*>& GetAllComponents() const { return m_Components; }

	template<class T>
	bool HasComponent()
	{
		for (auto Comp : m_Components)
		{
			if (Comp && Comp->GetID() == T::GetID())
			{
				return true;
			}
		}

		return false;
	}

	template<class T>
	T* GetComponent()
	{
		for (auto Comp : m_Components)
		{
			if (Comp && Comp->GetID() == T::GetID())
			{
				return static_cast<T*>(Comp);
			}
		}

		return nullptr;
	}

	template<class T>
	T* AddComponent()
	{
		return nullptr;
	}

	void AddComponent(ComponentBase* Component)
	{
		assert(Component);
		m_Components.insert(Component);
	}

	void RemoveComponent(ComponentBase* Component)
	{
		m_Components.erase(Component);
	}

	template<class T>
	void RemoveComponent()
	{
		for (auto It = m_Components.begin(); It != m_Components.end(); ++It)
		{
			if ((*It)->GetID() == T::GetID())
			{
				m_Components.erase(It);
				break;
			}
		}
	}

	void RemoveAllComponents()
	{
		m_Components.clear();
		m_ComponentHashes.clear();
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_ID),
			CEREAL_NVP(m_Parent),
			CEREAL_NVP(m_Child),
			CEREAL_NVP(m_Sibling),
			CEREAL_NVP(m_Visible),
			CEREAL_NVP(m_Selected),
			CEREAL_NVP(m_Name),
			CEREAL_NVP(m_ComponentHashes),
		);
	}
protected:
	friend class SceneGraph;

	inline void SetID(EntityID ID) { m_ID = ID; }
	
	inline bool IsAlive() const { return m_Alive; }
	inline Entity& SetAlive(bool Alive) { m_Alive = Alive; return *this; }
private:
	inline const std::unordered_set<size_t>& GetComponentHashes() const { return m_ComponentHashes; }

	EntityID m_ID;
	EntityID m_Parent;
	EntityID m_Child;
	EntityID m_Sibling;

	bool m_Alive = true;
	bool m_Visible = true;
	bool m_Selected = false;

	std::string m_Name;

	std::unordered_set<ComponentBase*> m_Components;
	std::unordered_set<size_t> m_ComponentHashes;
};

class SceneGraph
{
public:
	using EntityList = std::vector<Entity>;

	EntityID AddSibling(EntityID Sibling, const char* Name)
	{
		assert(Sibling.IsValid() && Sibling.GetIndex() < m_Entities.size());
		EntityID::IndexType SiblingIndex = Sibling.GetIndex();
		while (m_Entities[SiblingIndex].HasSibling())
		{
			SiblingIndex = m_Entities[SiblingIndex].GetSibling().GetIndex();
		}

		EntityID SiblingNode = AddEntity(m_Entities[SiblingIndex].GetParent(), Name);
		m_Entities[Sibling.GetIndex()].SetSibling(SiblingNode);
		return SiblingNode;
	}

	EntityID AddChild(EntityID Parent, const char* Name)
	{
		assert(Parent.IsValid() && Parent.GetIndex() < m_Entities.size());
		if (m_Entities[Parent.GetIndex()].HasChild())
		{
			return AddSibling(m_Entities[Parent.GetIndex()].GetChild(), Name);
		}
		else
		{
			auto NextID = AddEntity(Parent, Name);
			m_Entities[Parent.GetIndex()].SetChild(NextID);
			return NextID;
		}
	}

	EntityID AddEntity(EntityID Parent, const char* Name)
	{
		EntityID ID = EntityID(static_cast<EntityID::IndexType>(m_Entities.size()));
		m_Entities.emplace_back(Entity(Name, ID, Parent));
		return ID;
	}

	void RemoveEntity(EntityID ID)
	{
		assert(ID.IsValid() && ID.GetIndex() < m_Entities.size());
		m_Entities[ID.GetIndex()].RemoveAllComponents();
		m_Entities[ID.GetIndex()].SetAlive(false);
	}

	void RemoveDyingEntities()
	{
		std::vector<Entity> AliveEntities;
		AliveEntities.reserve(m_Entities.size());
		for (auto It = m_Entities.begin(); It != m_Entities.end(); ++It)
		{
			if (It->IsAlive())
			{
				AliveEntities.emplace_back(std::move(*It));
			}
		}
		std::swap(m_Entities, AliveEntities);
	}

	const Entity* FindEntityByName(const char* Name) const
	{
		for (auto& Entity : m_Entities)
		{
			if (strcmp(Entity.GetName(), Name) == 0)
			{
				return &Entity;
			}
		}
		return nullptr;
	}

	const Entity* GetEntity(const EntityID& ID) const
	{ 
		assert(ID.IsValid() && ID.GetIndex() < m_Entities.size());
		return m_Entities[ID.GetIndex()].IsAlive() ? &m_Entities[ID.GetIndex()] : nullptr;
	}

	inline EntityID GetRoot() const { return m_Root; }
	inline const EntityList& GetAllEntities() const { return m_Entities; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Root),
			CEREAL_NVP(m_Entities)
		);
	}
private:
	friend class AssimpSceneImporter;

	void SetRoot(EntityID ID) { m_Root = ID; }

	EntityID m_Root;
	EntityList m_Entities;
};
