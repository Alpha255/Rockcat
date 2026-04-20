#pragma once

#include "Core/Cereal.h"
#include "Scene/Components/ComponentBase.h"

class SkeletalMeshComponent : public ComponentBase
{
public:
	REGISTER_COMPONENT(SkeletalMeshComponent, ComponentBase);
};