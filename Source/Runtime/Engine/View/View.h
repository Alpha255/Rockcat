#pragma once

#include "Core/Definitions.h"
#include "Engine/RHI/RHIRenderStates.h"

class IView
{
public:
	IView() = default;
	IView(const struct GraphicsSettings& GfxSettings);

	virtual bool IsStereo() const { return false; }
	virtual bool IsInverseDepth() const { return false; }

	inline void SetViewport(const RHIViewport& Viewport) { m_Viewport = Viewport; }
	inline const RHIViewport& GetViewport() const { return m_Viewport; }

	inline const RHIScissorRect& GetScissorRect() const { return m_ScissorRect; }
	inline void SetScissorRect(const RHIScissorRect& ScissorRect) { m_ScissorRect = ScissorRect; }
private:
	RHIViewport m_Viewport;
	RHIScissorRect m_ScissorRect;
};

class PlanarView : public IView
{
public:
	using IView::IView;
};

class StereoView : public IView
{
public:
	using IView::IView;

	bool IsStereo() const override final { return true; }
};
