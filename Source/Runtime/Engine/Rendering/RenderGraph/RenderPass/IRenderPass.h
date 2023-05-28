#pragma once

#include "Colorful/IRenderer/IImage.h"
#include "Colorful/IRenderer/IBuffer.h"
#include "Runtime/Core/DirectedAcyclicGraph.h"

NAMESPACE_START(RHI)

struct RenderPassPort
{
	enum class EUsageFlags
	{
		Internal = 1 << 0,
		Input = 1 << 1,
		Output = 1 << 2,
	};

	enum class EType
	{
		Image,
		Buffer
	};

	EUsageFlags Usages = EUsageFlags::Internal;
	EType Type = EType::Image;
	std::string_view Name;
	std::variant<ImageDesc, BufferDesc> Description;
};
using RenderPortID = Handle<RenderPassPort, uint16_t>;

class IRenderPass : public DirectedAcyclicGraph::Node
{
public:
	IRenderPass(const char8_t* Name, class RenderGraphContext* Context)
		: m_Name(Name)
		, m_Context(Context)
	{
		assert(m_Context);
	}

	virtual ~IRenderPass() = default;

	virtual void Setup() {}

	virtual void ApplyRenderSettings(const struct RenderSettings* /*Settings*/) {};

	virtual void Execute() = 0;

	RenderPassPort& AddPort(RenderPassPort::EUsageFlags Usages, RenderPassPort::EType Type, const char8_t* Name)
	{
		assert(Name);
		m_Ports.emplace_back(RenderPassPort{ Usages, Type, Name });
		return m_Ports.back();
	}

	void Connect(const RenderPortID& Port);

	const char8_t* Name() const
	{
		return m_Name.data();
	}
protected:
	std::string_view m_Name;
	class RenderGraphContext* m_Context = nullptr;
	std::vector<RenderPassPort> m_Ports;
};
using RenderPassID = DirectedAcyclicGraph::NodeID;

NAMESPACE_END(RHI)
