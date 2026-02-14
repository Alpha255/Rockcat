#pragma once

#include "Core/ObjectID.h"
#include "Scene/Components/ComponentBase.h"

using EntityID = ObjectID<class Entity>;

class Entity
{
public:
	Entity() = default;

	Entity(FName&& Name, EntityID ID, EntityID Parent = EntityID())
		: m_ID(ID)
		, m_Parent(Parent)
		, m_Name(std::move(Name))
	{
	}

	Entity(class Scene* InScene, FName&& Name, EntityID ID, EntityID Parent = EntityID())
		: m_Scene(InScene)
		, m_ID(ID)
		, m_Parent(Parent)
		, m_Name(std::move(Name))
	{
	}

	Entity(const Entity&) = default;
	Entity(Entity&&) = default;
	Entity& operator=(const Entity&) = default;

	inline bool IsValid() const { return m_ID.IsValid(); }

	inline EntityID GetID() const { return m_ID; }

	inline bool HasParent() const { return m_Parent.IsValid(); }
	inline EntityID GetParent() const { return m_Parent; }
	inline Entity& SetParent(EntityID ParentID) { m_Parent = ParentID; return *this; }
	inline Entity& SetParent(const Entity& Parent) { m_Parent = Parent.GetID(); return *this; }

	inline bool HasChild() const { return m_Child.IsValid(); }
	inline EntityID GetChild() const { return m_Child; }
	inline Entity& SetChild(EntityID ChildID) { m_Child = ChildID; return *this; }
	inline Entity& SetChild(const Entity& Child) { m_Child = Child.GetID(); return *this; }

	inline bool HasSibling() const { return m_Sibling.IsValid(); }
	inline EntityID GetSibling() const { return m_Sibling; }
	inline Entity& SetSibling(EntityID SiblingID) { m_Sibling = SiblingID; return *this; }
	inline Entity& SetSibling(const Entity& Sibling) { m_Sibling = Sibling.GetID(); return *this; }

	inline bool IsVisible() const { return m_Visible; }
	inline Entity& SetVisible(bool Visible) { m_Visible = Visible; return *this; }

	inline bool IsSelected() const { return m_Selected; }
	inline Entity& SetSelected(bool Selected) { m_Selected = Selected; return *this; }

	inline const FName& GetName() const { return m_Name; }
	inline Entity& SetName(FName&& Name) { m_Name = std::move(Name); return *this; }

	inline bool IsAlive() const { return m_Alive; }

	template<class T>
	bool HasComponent()
	{
		for (auto Comp : m_Components)
		{
			if (Comp && Comp->GetID() == T::ID)
			{
				return true;
			}
		}

		return false;
	}

	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		for (auto Comp : m_Components)
		{
			if (Comp && Comp->GetID() == T::ID)
			{
				return Comp;
			}
		}

		return nullptr;
	}

	template<class T>
	std::vector<std::shared_ptr<T>> GetComponents()
	{
		std::vector<std::shared_ptr<T>> Components;
		for (auto Comp : m_Components)
		{
			if (Comp && Comp->GetID() == T::ID)
			{
				Components.push_back(Cast<T>(Comp));
			}
		}

		return Components;
	}

	template<class T, class... Args>
	inline std::shared_ptr<T> AddComponent(Args&&... InArgs)
	{
		return Cast<T>(m_Components.emplace_back(ComponentPool::Get().Allocate<T>(this, std::forward<Args>(InArgs)...)));
	}

	template<class T>
	void RemoveComponent()
	{
		for (auto It = m_Components.begin(); It != m_Components.end(); ++It)
		{
			if (Cast<T>((*It))->GetID() == T::ID)
			{
				m_Components.erase(It);
				break;
			}
		}
	}

	template<class T>
	void RemoveComponents()
	{
		for (auto It = m_Components.begin(); It != m_Components.end(); ++It)
		{
			if (Cast<T>((*It))->GetID() == T::ID)
			{
				It = m_Components.erase(It);
			}
		}
	}

	void RemoveAllComponents()
	{
		m_Components.clear();
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
			CEREAL_NVP(m_Components)
		);
	}
protected:
	friend class SceneGraph;
	friend class Scene;

	inline void SetID(EntityID ID) { m_ID = std::move(ID); }
	
	inline Entity& SetAlive(bool Alive) { m_Alive = Alive; return *this; }

	inline const std::vector<std::shared_ptr<ComponentBase>>& GetAllComponents() const { return m_Components; }
private:
	EntityID m_ID;
	EntityID m_Parent;
	EntityID m_Child;
	EntityID m_Sibling;

	bool m_Alive = true;
	bool m_Visible = true;
	bool m_Selected = false;

	FName m_Name;

	class Scene* m_Scene = nullptr;

	std::vector<std::shared_ptr<ComponentBase>> m_Components;
};

class SceneGraph
{
public:
	inline Entity& AddSibling(Entity& Sibling, FName&& Name)
	{
		assert(Sibling.IsValid() && Sibling.GetID().GetIndex() < m_Entities.size());

		auto NextSibing = &Sibling;
		while (NextSibing->HasSibling())
		{
			NextSibing = GetEntity(NextSibing->GetSibling());
		}

		Entity& Node = AddEntity(NextSibing->GetParent(), std::move(Name));
		NextSibing->SetSibling(Node);
		return Node;
	}

	Entity& AddSibling(EntityID SiblingID, FName&& Name)
	{
		return AddSibling(*GetEntity(SiblingID), std::move(Name));
	}

	inline Entity& AddChild(Entity& Parent, FName&& Name)
	{
		assert(Parent.IsValid() && Parent.GetID().GetIndex() < m_Entities.size());

		if (Parent.HasChild())
		{
			return AddSibling(Parent.GetChild(), std::move(Name));
		}

		Entity& Node = AddEntity(Parent.GetID(), std::move(Name));
		Parent.SetChild(Node.GetID());
		return Node;
	}

	Entity& AddChild(EntityID ParentID, FName&& Name)
	{
		return AddChild(*GetEntity(ParentID), std::move(Name));
	}

	Entity& AddEntity(EntityID Parent, FName&& Name)
	{
		EntityID ID = EntityID(static_cast<EntityID::IndexType>(m_Entities.size()));
		return m_Entities.emplace_back(Entity(std::move(Name), ID, Parent));
	}

	void RemoveEntity(EntityID ID)
	{
		if (ID.IsValid() && ID.GetIndex() < m_Entities.size())
		{
			m_Entities[ID.GetIndex()].SetAlive(false)
				.RemoveAllComponents();
		}
	}

	void RemoveInvalidEntities()
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

	const Entity* GetEntity(FName&& Name) const
	{
		for (auto& Entity : m_Entities)
		{
			if (Entity.GetName() == Name)
			{
				return &Entity;
			}
		}
		return nullptr;
	}

	Entity* GetEntity(FName&& Name)
	{
		for (auto& Entity : m_Entities)
		{
			if (Entity.GetName() == Name)
			{
				return &Entity;
			}
		}
		return nullptr;
	}

	const Entity* GetEntity(const EntityID& ID) const
	{
		assert(ID.IsValid() && ID.GetIndex() < m_Entities.size());
		return &m_Entities[ID.GetIndex()];
	}

	Entity* GetEntity(const EntityID& ID)
	{
		assert(ID.IsValid() && ID.GetIndex() < m_Entities.size());
		return &m_Entities[ID.GetIndex()];
	}

	inline const Entity* GetRoot() const { return m_Root.IsValid() ? GetEntity(m_Root) : nullptr; }
	inline Entity* GetRoot() { return m_Root.IsValid() ? GetEntity(m_Root) : nullptr; }
	inline const std::vector<Entity>& GetAllEntities() const { return m_Entities; }
	inline uint32_t GetNumEntity() const { return static_cast<uint32_t>(m_Entities.size()); }
	inline bool IsEmpty() const { return m_Entities.empty(); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Root),
			CEREAL_NVP(m_Entities)
		);
	}

protected:
	friend class AssimpSceneLoader;

	inline void SetRoot(EntityID ID) { m_Root = ID; }

	inline void SetRoot(const Entity& Node) { return SetRoot(Node.GetID()); }

	inline void SetEntityID(Entity& Node, EntityID ID) { Node.SetID(ID); }

	inline std::vector<Entity>& GetAllEntities() { return m_Entities; }

	friend void Merge(SceneGraph& Dest, const SceneGraph& Src);
	friend void Integrate(SceneGraph& Dest, SceneGraph& Src);
private:
	EntityID m_Root;
	std::vector<Entity> m_Entities;
};
