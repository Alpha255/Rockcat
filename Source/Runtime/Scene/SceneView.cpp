#include "Scene/SceneView.h"
#include "Scene/Components/Camera.h"

void ISceneView::SetCamera(const Camera* Camera)
{
	m_Camera = Camera;
}

const Math::Matrix& ISceneView::GetWorldMartix() const
{
	assert(m_Camera);
	return m_Camera->GetWorldMatrix();
}

const Math::Matrix& ISceneView::GetViewMatrix() const
{
	assert(m_Camera);
	return m_Camera->GetViewMatrix();
}

const Math::Matrix& ISceneView::GetProjectionMatrix() const
{
	assert(m_Camera);
	return m_Camera->GetProjectionMatrix();
}

const Math::Vector3& ISceneView::GetViewOriginPosition() const
{
	assert(m_Camera);
	return m_Camera->GetEyePosition();
}

Math::Frustum ISceneView::GetFrustum() const
{
	return Math::Frustum();
}

void PlanarSceneView::SetViewport(const RHIViewport& Viewport)
{
	m_Viewports[0] = Viewport;
}

void PlanarSceneView::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	m_ScissorRects[0] = ScissorRect;
}

void StereoSceneView::SetViewport(const RHIViewport& Viewport)
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

void StereoSceneView::SetScissorRect(const RHIScissorRect& ScissorRect)
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

void CubemapSceneView::SetViewport(const RHIViewport& Viewport)
{
	for (size_t Index = 0u; Index < m_Viewports.size(); ++Index)
	{
		m_Viewports[Index] = Viewport;
	}
}

void CubemapSceneView::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	for (size_t Index = 0u; Index < m_ScissorRects.size(); ++Index)
	{
		m_ScissorRects[Index] = ScissorRect;
	}
}
