#pragma once

#include "Core/Definitions.h"

using ComponentID = uint64_t;

#define DECLARE_COMPONENT_ID(ComponentType) \
private: \
	enum class EComponentType : ComponentID { \
 		ID = FnvHash(#ComponentType) \
	}; \
public: \
	static inline constexpr ComponentID GetID() { return static_cast<ComponentID>(EComponentType::ID); }

class ComponentBase
{
public:
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Hash)
		);
	}
private:
	inline void SetHash(size_t Hash) { m_Hash = Hash; }
	friend class ComponentPool;
	size_t m_Hash = 0u;
};