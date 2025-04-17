#pragma once

#include "Core/Singleton.h"

using ComponentID = uint64_t;

#define COMPONENT_HASH(ComponentType) FnvHash(#ComponentType)

#define COMPONENT_TYPE_DECLARE(ComponentType) \
	enum class EComponentType : ComponentID { \
 		ID = COMPONENT_HASH(ComponentType) \
	}; \
	static inline constexpr ComponentID GetID() { return static_cast<ComponentID>(EComponentType::ID); }

struct ComponentBase
{
private:
	friend class ComponentPool;
	uint32_t Index = ~0u;
};

class ComponentPool : public Singleton<ComponentPool>
{
public:
	template<class T, class... Args>
	T* Allocate(Args&&... ArgList)
	{
		uint32_t Index = ~0u;

		if (!m_FreedComponents.empty())
		{
			Index = m_FreedComponents.front();
			m_FreedComponents.pop_front();
			m_Components[Index] = std::make_shared<T>(std::forward<Args>(ArgList)...);
		}
		else
		{
			Index = static_cast<uint32_t>(m_Components.size());
			m_Components.emplace_back(std::make_shared<T>(std::forward<Args>(ArgList)...));
		}

		return static_cast<T*>(m_Components[Index].get());
	}

	template<class T>
	void Free(T*& Component)
	{
		const uint32_t Index = Component->Index;
		assert(Index < m_Components.size());
		m_FreedComponents.push_back(Index);
		m_Components[Index].reset();
		Component = nullptr;
	}
private:
	std::vector<std::shared_ptr<ComponentBase>> m_Components;
	std::list<uint32_t> m_FreedComponents;
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