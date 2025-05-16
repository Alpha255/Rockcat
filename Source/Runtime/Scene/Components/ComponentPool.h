#pragma once

#include "Scene/Components/ComponentBase.h"

class ComponentPool
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

class SkeletalMeshComponent : public ComponentBase
{
public:
	DECLARE_COMPONENT_ID(SkeletalMeshComponent)
};