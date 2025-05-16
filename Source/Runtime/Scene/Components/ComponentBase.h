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
private:
	friend class ComponentPool;
	uint32_t Index = ~0u;
};