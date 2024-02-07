#pragma once

#include "Core/Definitions.h"
#include "Engine/RHI/RHIRenderStates.h"

class IView
{
public:
	virtual bool IsStereo() const { return false; }
	virtual bool IsReverseDepth() const { return false; }

	const std::vector<RHIViewport>& GetViewports() const { return m_Viewports; }
	const std::vector<RHIScissorRect> GetScissorRects() const { return m_Scissors; }
private:
	std::vector<RHIViewport> m_Viewports;
	std::vector<RHIScissorRect> m_Scissors;
};

class View : public IView
{
};

class StereoView : public IView
{
};
