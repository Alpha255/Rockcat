#pragma once

#include "Colorful/IRenderer/RenderGraph/RenderPass.h"

NAMESPACE_START(RHI)

class ResourcePool
{
public:
    std::shared_ptr<IHWResource> Allocate(const Field& Property);

    std::shared_ptr<IHWResource> Find(const char8_t* Name);
protected:
private:
    std::unordered_map<const char8_t*, std::shared_ptr<IHWResource>> m_ResourceCache;
};

NAMESPACE_END(RHI)