#pragma once

#include "Core/Definitions.h"

using ComponentID = uint64_t;

#define DECLARE_COMPONENT_ID(ComponentType) \
private: \
	enum class EComponentType : ComponentID { \
 		ID = FnvHash(#ComponentType) \
	}; \
public: \
	static inline constexpr ComponentID GetID() { return static_cast<ComponentID>(EComponentType::ID); } \
	ComponentID GetComponentID() const override final { return static_cast<ComponentID>(EComponentType::ID); }

class ComponentBase
{
public:
	ComponentBase() = default;

	ComponentBase(const class Entity* Owner)
		: m_Owner(Owner)
	{
	}

	virtual ~ComponentBase() = default;

	inline const class Entity* GetOwner() const { return m_Owner; }

	virtual ComponentID GetComponentID() const { return 0u; }

	virtual void Tick(float /*ElapsedSeconds*/) {}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Hash)
		);
	}
private:
	friend class ComponentPool;

	inline void SetHash(size_t Hash) { m_Hash = Hash; }
	inline void SetOwner(const class Entity* Owner) { m_Owner = Owner; }
	
	size_t m_Hash = 0u;
	const class Entity* m_Owner = nullptr;
};