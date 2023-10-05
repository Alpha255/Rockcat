#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"

Field& ResourceManager::GetOrAllocateField(
	const char8_t* Name, 
	Field::EVisibility Visibility, 
	Field::EResourceType Type, 
	FieldID RefID)
{
	if (RefID.IsValid())
	{
		return m_Fields[RefID.GetIndex()];
	}

	m_Fields.emplace_back(Field(m_FieldIDAllocator.Allocate(), Name, Visibility, Type));
	return m_Fields.back();
}
