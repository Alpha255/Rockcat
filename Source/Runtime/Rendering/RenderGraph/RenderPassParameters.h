#pragma once

#include "Core/DirectedAcyclicGraph.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHIRenderStates.h"
#include "Scene/SceneView.h"
#include "Rendering/RenderSettings.h"

using DAGNodeID = DirectedAcyclicGraph::NodeID;
using RDGResourceID = ObjectID<class RDGResource>;

class RDGResource
{
public:
	RDGResource(const RDGResource&) = delete;
	virtual ~RDGResource() = default;

	RHIResource* GetRHI() const { return m_RHIResource.get(); }
protected:
	RDGResource(const char* const Name)
		: m_Name(Name)
	{
	}

private:
	const char* const m_Name;
	RHIResourcePtr m_RHIResource = nullptr;
};

class RDGBuffer : public RDGResource
{
public:
	using RDGResource::RDGResource;

	RHIBuffer* GetRHI() const { return static_cast<RHIBuffer*>(RDGResource::GetRHI()); }
};

template<class Parameters>
class RDGUniformBuffer : public RDGBuffer
{
public:
	Parameters& GetParameters() { return *m_Parameters; }

	RDGUniformBuffer(const char* const Name)
		: RDGBuffer(Name)
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

	inline RHITextureDesc& GetDesc() { return m_Desc; }
private:
	RHISubresource m_Subresource;
	RHITextureDesc m_Desc;
};

struct RDGTextureMSAA
{
	RDGTexture* Target = nullptr;
	RDGTexture* ResolveTarget = nullptr;

	inline RDGTextureMSAA& SetTarget(RDGTexture* InTarget)
	{
		Target = InTarget;
		return *this;
	}

	inline RDGTextureMSAA& SetResolveTarget(RDGTexture* InTarget)
	{
		ResolveTarget = InTarget;
		return *this;
	}
};

struct RDGRenderTargetBinding
{
	RDGTexture* Texture = nullptr;
	RDGTexture* ResolveTexture = nullptr;

	uint16_t MipLevel = 0u;
	uint16_t ArrayLayer = 0u;

	ERHILoadOp LoadOp = ERHILoadOp::DontCare;

	inline bool Validate() const
	{
		return Texture && RHI::IsColor(Texture->GetDesc().Format);
	}

	inline RDGRenderTargetBinding& SetTexture(RDGTexture* InTexture)
	{
		Texture = InTexture;
		assert(Validate());
		return *this;
	}

	inline RDGRenderTargetBinding& SetResolveTexture(RDGTexture* InTexture)
	{
		ResolveTexture = InTexture;
		assert(Validate());
		return *this;
	}

	inline RDGRenderTargetBinding& SetMipLevel(uint16_t InMipLevel)
	{
		MipLevel = InMipLevel;
		return *this;
	}

	inline RDGRenderTargetBinding& SetArrayLayer(uint16_t InArrayLayer)
	{
		ArrayLayer = InArrayLayer;
		return *this;
	}

	inline RDGRenderTargetBinding& SetLoadOp(ERHILoadOp InLoadOp)
	{
		LoadOp = InLoadOp;
		return *this;
	}
};

struct RDGDepthStencilBinding : public RDGRenderTargetBinding
{
	ERHILoadOp StencilLoadOp = ERHILoadOp::DontCare;

	inline bool Validate() const
	{
		return Texture && RHI::IsDepthStencil(Texture->GetDesc().Format);
	}

	inline RDGDepthStencilBinding& SetDepthLoadOp(ERHILoadOp InDepthLoadOp)
	{
		LoadOp = InDepthLoadOp;
		return *this;
	}

	inline RDGDepthStencilBinding& SetStencilLoadOp(ERHILoadOp InStencilLoadOp)
	{
		StencilLoadOp = InStencilLoadOp;
		return *this;
	}
};

struct RDGRenderTargetBindingSlots
{
	std::array<RDGRenderTargetBinding, ERHILimitations::MaxRenderTargets> Colors;
	RDGDepthStencilBinding DepthStencil;
};

struct RDGSceneTextures
{
	struct RDGGBuffers
	{
		RDGTexture* BaseColor;
		RDGTexture* WorldNormal;
		RDGTexture* Metallic;
		RDGTexture* Roughness;
	};

	void InitializeWithSceneView(class RDGRenderGraph& Graph, const SceneView& View);

	uint32_t GetSceneTextureBindingSlots(RDGRenderTargetBindingSlots& Slots, bool WithDepthStencil = true) const;
	uint32_t GetGBufferBindingSlots(RDGRenderTargetBindingSlots& Slots) const;

	RDGTextureMSAA Color;
	RDGTextureMSAA Depth;

	RDGGBuffers GBuffers;

	RDGTexture* Velocity;

	RDGTexture* ScreenSpacAO;
};

struct RDGSceneViewInfo
{
	RDGSceneViewInfo(class RDGRenderGraph& Graph, const class RHIViewWindow& ViewWindow);

	Math::UInt2 OriginalViewSize;
	Math::UInt2 FinalViewSize;

	ViewRect OriginalViewRect;
	ViewRect FinalViewRect;

	RDGSceneTextures SceneTextures;

	RHITexture* FinalOutput;

	std::vector<SceneView> Views;
};

class RDGRenderPassParameters
{

};