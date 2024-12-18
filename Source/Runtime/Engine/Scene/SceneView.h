#pragma once

#include "Core/Definitions.h"
#include "Engine/RHI/RHIRenderStates.h"

class SceneView
{
public:
	SceneView() = default;
	SceneView(const struct GraphicsSettings& GfxSettings);

	virtual bool IsStereo() const { return false; }

	inline bool IsInverseDepth() const { return m_InverseDepth; }
	inline void SetInverseDepth(bool InverseDepth) { m_InverseDepth = InverseDepth; }

	inline void SetViewport(const RHIViewport& Viewport) { m_Viewport = Viewport; }
	inline const RHIViewport& GetViewport() const { return m_Viewport; }

	inline const RHIScissorRect& GetScissorRect() const { return m_ScissorRect; }
	inline void SetScissorRect(const RHIScissorRect& ScissorRect) { m_ScissorRect = ScissorRect; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_InverseDepth),
			CEREAL_NVP(m_Viewport),
			CEREAL_NVP(m_ScissorRect)
		);
	}
private:
	bool m_InverseDepth = false;
	RHIViewport m_Viewport;
	RHIScissorRect m_ScissorRect;
};

class PlanarSceneView : public SceneView
{
public:
	using SceneView::SceneView;
};

class StereoSceneView : public SceneView
{
public:
	using SceneView::SceneView;

	bool IsStereo() const override final { return true; }
};
