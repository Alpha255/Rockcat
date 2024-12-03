#pragma once

#include "Core/Definitions.h"
#include "Engine/RHI/RHIRenderStates.h"

class IView
{
public:
	virtual bool IsStereo() const { return false; }
	virtual bool IsReverseDepth() const { return false; }

	const RHIViewport& GetViewport() const { return m_Viewport; }
	const RHIScissorRect& GetScissorRect() const { return m_Scissor; }
private:
	RHIViewport m_Viewport;
	RHIScissorRect m_Scissor;
};

class PlanarView : public IView
{
};

class StereoView : public IView
{
};
