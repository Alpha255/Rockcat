#pragma once

#include "Core/DirectedAcyclicGraph.h"
#include "RHI/RHIBuffer.h"

using DAGNodeID = DirectedAcyclicGraph::NodeID;
using RDGResourceID = ObjectID<class RDGResource>;

class RDGResource
{
public:
	RDGResource(const RDGResource&) = delete;
	virtual ~RDGResource() = default;

	RHIResource* GetRHI() const { return m_RHIResource; }
protected:
	RDGResource(const char* const Name)
		: m_Name(Name)
	{
	}

private:
	const char* const m_Name;
	RHIResource* m_RHIResource = nullptr;
};

template<class Parameters>
class RDGUniformBuffer : public RDGResource
{
public:
	RHIBuffer* GetRHI() const { return static_cast<RHIBuffer*>(RDGResource::GetRHI()); }

	Parameters& GetParameters() { return *m_Parameters; }

	RDGUniformBuffer(const char* const Name)
		: RDGResource(Name)
		, m_Parameters(new Parameters())
	{
	}
private:
	std::shared_ptr<Parameters> m_Parameters;
};

class RDGTexture : public RDGResource
{
public:
	RHITexture* GetRHI() const { return static_cast<RHITexture*>(RDGResource::GetRHI()); }

	const RHISubresource& GetSubresource() const { return m_Subresource; }

	uint32_t ComputeMemorySize() const;
private:
	RHISubresource m_Subresource;
};

class RDGRenderTarget : public RDGTexture
{
};

class RDGRenderPassParameters
{

};