#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class RenderGraphCompiler
{
public:
    RenderGraphCompiler(class RenderGraph& Graph);

    void Compile();
protected:
    void InsertResourceBarrierPass();
    void InsertResolvePass();
private:
    class RenderGraph& m_Graph;
    std::vector<std::shared_ptr<RenderPass>> m_Passes;
};