#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

Field& Field::SetVisibility(EVisibility Visibility)
{
	m_Visibility = (Visibility | m_Visibility);
	return *this;
}
