#include "Scene/SceneView.h"
#include "Scene/Components/Camera.h"

void SceneView::SetCamera(Camera* Camera)
{
	m_Camera = Camera;
}

const Math::Matrix& SceneView::GetWorldMartix() const
{
	assert(m_Camera);
	return m_Camera->GetWorldMatrix();
}

const Math::Matrix& SceneView::GetViewMatrix() const
{
	assert(m_Camera);
	return m_Camera->GetViewMatrix();
}

const Math::Matrix& SceneView::GetProjectionMatrix() const
{
	assert(m_Camera);
	return m_Camera->GetProjectionMatrix();
}

const Math::Vector3& SceneView::GetViewOriginPosition() const
{
	assert(m_Camera);
	return m_Camera->GetEyePosition();
}

Math::Frustum SceneView::GetFrustum() const
{
	return Math::Frustum();
}

void SceneView::SetDefaultScissorRect()
{
	if (m_ScissorRects.empty())
	{
		assert(!m_Viewports.empty());

		RHIScissorRect Scissor(static_cast<uint32_t>(m_Viewports[0].GetWidth()), static_cast<uint32_t>(m_Viewports[0].GetHeight()));
		SetScissorRect(Scissor);
	}
}

void SceneView::SetViewMode(EViewMode ViewMode)
{
	m_ViewMode = ViewMode;

	switch (ViewMode)
	{
	}
}

Math::UInt2 SceneView::GetViewSize() const
{
	return m_Viewports.empty() ? Math::UInt2(0u, 0u) : Math::UInt2(static_cast<uint32_t>(m_Viewports[0].GetWidth()), static_cast<uint32_t>(m_Viewports[0].GetHeight()));
}

void PlanarSceneView::SetViewport(const RHIViewport& Viewport)
{
	m_Viewports[0] = Viewport;
	SetDefaultScissorRect();
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

	SetDefaultScissorRect();
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

	SetDefaultScissorRect();
}

void CubemapSceneView::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	for (size_t Index = 0u; Index < m_ScissorRects.size(); ++Index)
	{
		m_ScissorRects[Index] = ScissorRect;
	}
}
