#pragma once

#include "Core/Definitions.h"
#include "Core/Math/Frustum.h"
#include "RHI/RHIRenderStates.h"

enum class EViewMode : uint8_t
{
	Lit,
	UnLit,
	Wireframe,
	VisualizeDepth,
	VisualizeGBuffer,
	VisualizeShadowMap,
};

class ISceneView
{
public:
	virtual ~ISceneView() = default;
	
	virtual bool IsStereoView() const { return false; }
	virtual bool IsCubemapView() const { return false; }

	virtual void SetViewport(const RHIViewport& Viewport) = 0;
	virtual void SetScissorRect(const RHIScissorRect& ScissorRect) = 0;

	virtual void SetCamera(class Camera* Camera);
	inline class Camera* GetCamera() { return m_Camera; }
	
	inline bool IsInverseDepth() const { return m_InverseDepth; }
	inline void SetInverseDepth(bool InverseDepth) { m_InverseDepth = InverseDepth; }

	inline bool IsMirrored() const { return m_Mirrored; }
	inline void SetMirrored(bool Mirrored) { m_Mirrored = Mirrored; }

	inline const std::vector<RHIViewport>& GetViewports() const { return m_Viewports; }
	inline const std::vector<RHIScissorRect>& GetScissorRects() const { return m_ScissorRects; }

	inline RHITexture* GetRenderSurface() const { return m_RenderSurface.get(); }

	inline EViewMode GetViewMode() const { return m_ViewMode; }

	const Math::Matrix& GetWorldMartix() const;
	const Math::Matrix& GetViewMatrix() const;
	const Math::Matrix& GetProjectionMatrix() const;
	const Math::Vector3& GetViewOriginPosition() const;
	Math::Frustum GetFrustum() const;

	void SetViewMode(EViewMode ViewMode);
protected:
	bool m_InverseDepth = false;
	bool m_Mirrored = false;

	EViewMode m_ViewMode = EViewMode::Lit;

	class Camera* m_Camera = nullptr;

	RHITexturePtr m_RenderSurface;

	std::vector<RHIViewport> m_Viewports;
	std::vector<RHIScissorRect> m_ScissorRects;
};

class PlanarSceneView : public ISceneView
{
public:
	using ISceneView::ISceneView;

	PlanarSceneView()
	{
		m_Viewports.resize(1u);
		m_ScissorRects.resize(1u);
	}

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
};

class StereoSceneView : public ISceneView
{
public:
	using ISceneView::ISceneView;

	enum class EView
	{
		Left,
		Right
	};

	StereoSceneView()
	{
		m_Viewports.resize(2u);
		m_ScissorRects.resize(2u);
	}

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;

	bool IsStereoView() const override final { return true; }
};

class CubemapSceneView : public ISceneView
{
public:
	using ISceneView::ISceneView;

	enum class EView
	{
		Up,
		Down,
		Left,
		Right,
		Front,
		Back
	};

	CubemapSceneView()
	{
		m_Viewports.resize(6u);
		m_ScissorRects.resize(6u);
	}

	bool IsCubemapView() const override final { return true; }

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
};
