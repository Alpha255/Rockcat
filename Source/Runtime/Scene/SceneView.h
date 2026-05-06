#pragma once

#include "Core/Definitions.h"
#include "Core/Math/Frustum.h"
#include "Core/Math/Rect.h"
#include "RHI/RHIRenderStates.h"

using ViewRect = Math::Rect<uint32_t>;

struct ViewMatrices
{
	Math::Matrix Projection;
	Math::Matrix InvProjection;
	Math::Matrix View;
	Math::Matrix InvView;
	Math::Matrix ViewProjection;
	Math::Matrix InvViewProjection;

	Math::Vector3 ViewOrigin;
};

class SceneView
{
public:
	enum class EViewMode : uint8_t
	{
		Lit,
		UnLit,
		Wireframe,
		VisualizeDepth,
		VisualizeGBuffer,
		VisualizeShadowMap,
	};

	enum class EProjectionMode : uint8_t
	{
		Perspective,
		Orthographic
	};

	virtual ~SceneView() = default;
	
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

	inline EViewMode GetViewMode() const { return m_ViewMode; }
	void SetViewMode(EViewMode ViewMode);

	inline EProjectionMode GetProjectionMode() const { return m_ProjectionMode; }

	inline const ViewRect& GetViewRect() const { return m_ViewRect; }

	const Math::Matrix& GetWorldMartix() const;
	const Math::Matrix& GetViewMatrix() const;
	const Math::Matrix& GetProjectionMatrix() const;
	const Math::Vector3& GetViewOriginPosition() const;
	Math::Frustum GetFrustum() const;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_InverseDepth),
			CEREAL_NVP(m_Mirrored),
			CEREAL_NVP(m_ViewMode),
			CEREAL_NVP(m_Viewports),
			CEREAL_NVP(m_ScissorRects)
		);
	}
protected:
	void SetDefaultScissorRect();

	bool m_InverseDepth = false;
	bool m_Mirrored = false;

	EViewMode m_ViewMode = EViewMode::Lit;
	EProjectionMode m_ProjectionMode = EProjectionMode::Perspective;

	ViewRect m_ViewRect;

	class Camera* m_Camera = nullptr;

	std::vector<RHIViewport> m_Viewports;
	std::vector<RHIScissorRect> m_ScissorRects;
};

class PlanarSceneView : public SceneView
{
public:
	using SceneView::SceneView;

	PlanarSceneView()
	{
		m_Viewports.resize(1u);
		m_ScissorRects.resize(1u);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneView)
		);
	}

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
};

class StereoSceneView : public SceneView
{
public:
	using SceneView::SceneView;

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

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneView)
		);
	}

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;

	bool IsStereoView() const override final { return true; }
};

class CubemapSceneView : public SceneView
{
public:
	using SceneView::SceneView;

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

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneView)
		);
	}

	bool IsCubemapView() const override final { return true; }

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
};
