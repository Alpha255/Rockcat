#include "Scene/SceneView.h"
#include "View/Camera.h"

void IView::SetCamera(const Camera* Camera)
{
	m_Camera = Camera;
}

const Math::Matrix& IView::GetWorldMartix() const
{
	assert(m_Camera);
	return m_Camera->GetWorldMatrix();
}

const Math::Matrix& IView::GetViewMatrix() const
{
	assert(m_Camera);
	return m_Camera->GetViewMatrix();
}

const Math::Matrix& IView::GetProjectionMatrix() const
{
	assert(m_Camera);
	return m_Camera->GetProjectionMatrix();
}

const Math::Vector3& IView::GetViewOriginPosition() const
{
	assert(m_Camera);
	return m_Camera->GetEyePosition();
}

Math::Frustum IView::GetFrustum() const
{
	return Math::Frustum();
}

void PlanarView::SetViewport(const RHIViewport& Viewport)
{
	m_Viewports[0] = Viewport;
}

void PlanarView::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	m_ScissorRects[0] = ScissorRect;
}

void StereoView::SetViewport(const RHIViewport& Viewport)
{
	m_Viewports[(size_t)EView::Left] = RHIViewport(
		Viewport.LeftTop.x, 
		Viewport.LeftTop.y, 
		Viewport.GetWidth() * 0.5f, 
		Viewport.GetHeight());

	m_Viewports[(size_t)EView::Right] = RHIViewport(
		Viewport.LeftTop.x + Viewport.GetWidth() * 0.5f,
		Viewport.LeftTop.y,
		Viewport.GetWidth() * 0.5f,
		Viewport.GetHeight());
}

void StereoView::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	m_ScissorRects[(size_t)EView::Left] = RHIScissorRect(
		ScissorRect.LeftTop.x,
		ScissorRect.LeftTop.y,
		static_cast<uint32_t>(ScissorRect.GetWidth() * 0.5f),
		ScissorRect.GetHeight());

	m_ScissorRects[(size_t)EView::Right] = RHIScissorRect(
		static_cast<int32_t>(ScissorRect.LeftTop.x + ScissorRect.GetWidth() * 0.5f),
		ScissorRect.LeftTop.y,
		static_cast<uint32_t>(ScissorRect.GetWidth() * 0.5f),
		ScissorRect.GetHeight());
}

void CubemapView::SetViewport(const RHIViewport& Viewport)
{
	for (size_t Index = 0u; Index < m_Viewports.size(); ++Index)
	{
		m_Viewports[Index] = Viewport;
	}
}

void CubemapView::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	for (size_t Index = 0u; Index < m_ScissorRects.size(); ++Index)
	{
		m_ScissorRects[Index] = ScissorRect;
	}
}
