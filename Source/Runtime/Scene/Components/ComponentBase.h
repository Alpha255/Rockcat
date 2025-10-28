#pragma once

#include "Core/Singleton.h"
#include "Core/Cereal.h"

using ComponentID = uint64_t;

#define REGISTER_COMPONENT_ID(ComponentType) \
private: \
	friend class ComponentPool; \
	inline static constexpr ComponentID ID = FnvHash(#ComponentType); \
public: \
	inline static constexpr ComponentID GetID() { return ID; } \

class ComponentBase : std::enable_shared_from_this<ComponentBase>
{
public:
	ComponentBase() = default;

	ComponentBase(class Entity* Owner)
		: m_Owner(Owner)
	{
	}

	virtual ~ComponentBase() = default;

	inline const class Entity& GetOwner() const { return *m_Owner; }

	virtual void Tick(float /*ElapsedSeconds*/) {}

	template<class Archive>
	void serialize(Archive&)
	{
	}
private:
	friend class ComponentPool;

	inline void SetOwner(class Entity* Owner) { m_Owner = Owner; }
	
	class Entity* m_Owner = nullptr;
};

class ComponentPool : public Singleton<ComponentPool>
{
public:
	template<class T, class... Args>
	std::shared_ptr<T> Allocate(Args&&... InArgs)
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "T must be derived from ComponentBase");

		auto& FreeComponents = m_FreeComponents[T::ID];
		if (FreeComponents.empty())
		{
			return std::make_shared<T>(std::forward<Args>(InArgs)...);
		}
		else
		{
			auto Component = Cast<T>(FreeComponents.front());
			FreeComponents.pop_front();
			new (Component.get()) T(std::forward<Args>(InArgs)...);
			return Component;
		}
	}

	template<class T>
	void Free(std::shared_ptr<T>& Component)
	{
		if (Component)
		{
			m_FreeComponents[T::ID].emplace_back(std::move(Component));
		}
	}
private:
	std::unordered_map<ComponentID, std::list<std::shared_ptr<ComponentBase>>> m_FreeComponents;
};

CEREAL_REGISTER_TYPE(ComponentBase);