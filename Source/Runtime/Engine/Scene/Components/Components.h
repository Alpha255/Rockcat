#pragma once

#include "Core/Definitions.h"

using ComponentID = uint64_t;
using ComponentList = std::unordered_map<ComponentID, struct ComponentBase*>;

#define COMPONENT_HASH(ComponentType) FnvHash(#ComponentType)

#define COMPONENT_TYPE_DECLARE(ComponentType) \
	enum class EComponentType : ComponentID { \
 		ID = COMPONENT_HASH(ComponentType) \
	}; \
	static inline constexpr ComponentID GetID() { return static_cast<ComponentID>(EComponentType::ID); }

struct ComponentBase
{
};

class ComponentPool
{
public:
	static ComponentPool& Get()
	{
		static ComponentPool s_Instance;
		return s_Instance;
	}

	template<class T, class... Args>
	T* Allocate(Args&&... ArgList)
	{
		auto It = m_FreedComponents.find(T::GetID());
		if (It != m_FreedComponents.end() && !It->second.empty())
		{
			auto* Component = It->second.front();
			It->second.pop_front();
			return new (Component) T(std::forward<Args>(ArgList)...);
		}

		auto& Component = m_Components.emplace_back(std::make_shared<T>(std::forward<Args>(ArgList)...));
		return static_cast<T*>(Component.get());
	}

	template<class T>
	void Free(T* Component)
	{
		auto It = m_FreedComponents.find(T::GetID());
		if (It == m_FreedComponents.end())
		{
			It = m_FreedComponents.emplace(T::GetID(), std::list<ComponentBase*>()).first;
		}
		It->second.push_back(Component);
	}
private:
	std::vector<std::shared_ptr<ComponentBase>> m_Components;
	std::unordered_map<ComponentID, std::list<ComponentBase*>> m_FreedComponents;
};

class TransformComponent : public ComponentBase
{
public:
	COMPONENT_TYPE_DECLARE(TransformComponent)
};

class StaticMeshComponent : public ComponentBase
{
public:
	COMPONENT_TYPE_DECLARE(StaticMeshComponent)
};

class SkeletalMeshComponent : public ComponentBase
{
public:
	COMPONENT_TYPE_DECLARE(SkeletalMeshComponent)
};

class CameraComponent : public ComponentBase
{
public:
	COMPONENT_TYPE_DECLARE(CameraComponent)
};

class Entity
{
public:
	template<class T, class... Args>
	T* AddComponent(Args&&... ArgList)
	{
		auto* Component = ComponentPool::Get().Allocate<T>(std::forward<Args>(ArgList)...);
		m_Components.emplace(T::GetID(), Component);
		return Component;
	}

	template<class T>
	T* GetComponent()
	{
		auto It = m_Components.find(T::GetID());
		if (It != m_Components.end())
		{
			return static_cast<T*>(It->second);
		}
		return nullptr;
	}
private:
	ComponentList m_Components;
};

template<class... Components>
class Entity2
{
public:
	using ComponentList = std::tuple<Components*...>;

	Entity2()
	{
		MakeComponent(m_Components);
	}

	template<class T>
	inline T* GetComponent()
	{
		return std::get<T*>(m_Components);
	}
private:
	template<size_t Index = 0u, class... T>
	void MakeComponent(std::tuple<T...>& Components)
	{
		if constexpr (Index < sizeof...(T))
		{
			auto* Component = ComponentPool::Get().Allocate<T>();
			std::get<Index>(Components) = Component;
			MakeComponent<Index + 1>(Components);
		}
	}

	ComponentList m_Components;
};