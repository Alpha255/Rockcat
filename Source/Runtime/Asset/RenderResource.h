#pragma once

#include "Core/Definitions.h"

class RenderResource
{
public:
	static void CreateGlobalResources();
	static void DestroyGlobalResources();

	virtual void CreateRHI() {}
	virtual void ReleaseRHI() {}
	virtual void Initialize();
private:
	uint32_t m_Index = NONE_INDEX;
};

template<class T>
class GlobalRenderResource : public T
{
public:
	GlobalRenderResource()
	{
		static_cast<T*>(this)->Initialize();
	}
};
