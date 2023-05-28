#pragma once

#include "Colorful/IRenderer/RenderGraph/RenderPass.h"

NAMESPACE_START(RHI)

class RenderGraphCompiler
{
public:
    RenderGraphCompiler(class RenderGraph& Graph)
        : m_Graph(Graph)
    {
    }

    void Compile();
protected:
    void ResolveExecutionList();
private:
    class RenderGraph& m_Graph;
    std::vector<std::shared_ptr<IRenderPass>> m_Passes;
};

NAMESPACE_END(RHI)