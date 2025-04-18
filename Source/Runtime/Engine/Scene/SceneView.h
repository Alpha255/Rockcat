#pragma once

#include "Core/Definitions.h"
#include "Core/Math/Frustum.h"
#include "Engine/RHI/RHIRenderStates.h"

class IView
{
public:
	virtual ~IView() = default;
	
	virtual bool IsStereoView() const { return false; }
	virtual bool IsCubemapView() const { return false; }

	virtual void SetViewport(const RHIViewport& Viewport) = 0;
	virtual void SetScissorRect(const RHIScissorRect& ScissorRect) = 0;

	virtual void SetCamera(const class Camera* Camera);
	
	inline bool IsInverseDepth() const { return m_InverseDepth; }
	inline void SetInverseDepth(bool InverseDepth) { m_InverseDepth = InverseDepth; }

	inline bool IsMirrored() const { return m_Mirrored; }
	inline void SetMirrored(bool Mirrored) { m_Mirrored = Mirrored; }

	inline const std::vector<RHIViewport>& GetViewports() const { return m_Viewports; }
	inline const std::vector<RHIScissorRect>& GetScissorRects() const { return m_ScissorRects; }

	const Math::Matrix& GetWorldMartix() const;
	const Math::Matrix& GetViewMatrix() const;
	const Math::Matrix& GetProjectionMatrix() const;
	const Math::Vector3& GetViewOriginPosition() const;
	Math::Frustum GetFrustum() const;
protected:
	bool m_InverseDepth = false;
	bool m_Mirrored = false;

	const class Camera* m_Camera = nullptr;

	std::vector<RHIViewport> m_Viewports;
	std::vector<RHIScissorRect> m_ScissorRects;
};

class PlanarView : public IView
{
public:
	using IView::IView;

	PlanarView()
	{
		m_Viewports.resize(1u);
		m_ScissorRects.resize(1u);
	}

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
};

class StereoView : public IView
{
public:
	using IView::IView;

	enum class EView
	{
		Left,
		Right
	};

	StereoView()
	{
		m_Viewports.resize(2u);
		m_ScissorRects.resize(2u);
	}

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;

	bool IsStereoView() const override final { return true; }
};

class CubemapView : public IView
{
public:
	using IView::IView;

	enum class EView
	{
		Up,
		Down,
		Left,
		Right,
		Front,
		Back
	};

	CubemapView()
	{
		m_Viewports.resize(6u);
		m_ScissorRects.resize(6u);
	}

	bool IsCubemapView() const override final { return true; }

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
};
