#include "Object2D.h"

#include "Image.h"
#include "Engine.h"

Object2D::Object2D(eType type, uint32_t width, uint32_t height, uint32_t left, uint32_t top)
	: m_Type(type)
	, m_ObjectWidth(width)
	, m_ObjectHeight(height)
{
	m_Image = Engine::Instance().GetImage(type);
	assert(m_Image);

	m_Area.Left = left;
	m_Area.Top = top;
}
