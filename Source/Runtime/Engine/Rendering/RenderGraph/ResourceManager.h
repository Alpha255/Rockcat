#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	Field& GetOrAllocateField(const char8_t* Name, Field::EVisibility Visibility, Field::EResourceType Type, 
		FieldID RefID = FieldID());
private:
	std::vector<Field> m_Fields;
	FieldIDAllocator m_FieldIDAllocator;
};