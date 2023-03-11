#pragma once

#include "Colorful/Vulkan/VulkanQueue.h"
#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanPipeline.h"

NAMESPACE_START(RHI)

struct VulkanExtensions
{
	bool8_t Synchronization2 = false;
	bool8_t ExtendedDynamicState = false;
	bool8_t ExtendedDynamicState2 = false;
	bool8_t TimelineSemaphore = false;
	bool8_t RenderPass2 = false;
	bool8_t FullscreenExclusive = false;
	bool8_t DebugUtils = false;
	bool8_t DebugReport = false;
	bool8_t DebugMarker = false;
	bool8_t Maintenance1 = false;
};

enum class ESyncType
{
	SyncByFence,
	QueueWaitSemaphore, /// Can't insure the texture inuse is complete upload when not use async transfer ???
	ForceWaitQueueIdle  /// Can't insure the texture inuse is complete upload when not use async transfer ???
};

class VulkanDevice final : public VkHWObject<IDevice, VkDevice_T>
{
public:
	VulkanDevice(class VulkanInstance* Instance);

	~VulkanDevice();

	const VulkanExtensions& EnabledExtensions() const
	{
		return m_Extensions;
	}

	VkPhysicalDevice PhysicalDevice() const
	{
		assert(m_PhysicalDevice);
		return m_PhysicalDevice;
	}

	VkInstance Instance() const
	{
		assert(m_Instance);
		return m_Instance;
	}

	const VkPhysicalDeviceLimits& PhysicalLimits() const
	{
		assert(IsValid());
		return m_PhysicalLimits;
	}

	const VkPhysicalDeviceSparseProperties& SparseProperties() const
	{
		assert(IsValid());
		return m_PhysicalDeviceSparseProperties;
	}

	const VkPhysicalDeviceFeatures& PhysicalFeatures() const
	{
		assert(IsValid());
		return m_Features;
	}

	void SetObjectName(uint64_t ObjectHandle, VkObjectType Type, const char8_t* Name);

	void WaitIdle() override final
	{
		VERIFY_VK(vkDeviceWaitIdle(Get()));
	}

	IShaderPtr CreateShader(const ShaderDesc& Desc) override final
	{
		return std::make_shared<VulkanShader>(this, Desc);
	}

	IImagePtr CreateImage(const ImageDesc& Desc) override final
	{
		return std::make_shared<VulkanImage>(this, Desc);
	}

	IInputLayoutPtr CreateInputLayout(const InputLayoutDesc& Desc, const ShaderDesc& VertexShaderDesc) override final
	{
		return std::make_shared<VulkanInputLayout>(Desc, VertexShaderDesc);
	}

	IFrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& Desc) override final
	{
		return std::make_shared<VulkanFramebuffer>(this, Desc);
	}

	IPipelinePtr CreateGraphicsPipeline(const GraphicsPipelineDesc& Desc) override final
	{
		return std::make_shared<VulkanGraphicsPipeline>(this, m_PipelineCache->Get(), Desc);
	}

	IBufferPtr CreateBuffer(const BufferDesc& Desc) override final
	{
		return std::make_shared<VulkanBuffer>(this, Desc);
	}

	ISamplerPtr CreateSampler(const SamplerDesc& Desc) override final
	{
		return std::make_shared<VulkanSampler>(this, Desc);
	}

	ICommandBufferPoolPtr CreateCommandBufferPool(EQueueType QueueType) override final
	{
		return Queue(QueueType)->CreateCommandBufferPool();
	}

	ICommandBufferPtr GetOrAllocateCommandBuffer(EQueueType QueueType, ECommandBufferLevel Level, bool8_t AutoBegin, bool8_t UseForTransfer) override final
	{
		return Queue(QueueType)->GetOrAllocateCommandBuffer(Level, AutoBegin, UseForTransfer);
	}

	VulkanDescriptor GetOrAllocateDescriptor(const GraphicsPipelineDesc& Desc);

	std::shared_ptr<VulkanFence> GetOrCreateFence(bool8_t Signaled = false);

	void FreeFence(std::shared_ptr<VulkanFence> Fence);

	VulkanQueue* Queue(EQueueType QueueType)
	{
		assert(QueueType <= EQueueType::Compute);

		static const uint32_t GraphicsQueueIndex = static_cast<uint32_t>(EQueueType::Graphics);
		const uint32_t QueueIndex = static_cast<uint32_t>(QueueType);

#if !USE_DEDICATE_TRANSFER_QUEUE
		if (QueueType == EQueueType::Transfer)
		{
			return m_Queues[GraphicsQueueIndex].get();
		}
#endif
		return m_Queues[QueueIndex] ? m_Queues[QueueIndex].get() : m_Queues[GraphicsQueueIndex].get();
	}

	void SubmitCommandBuffers(EQueueType QueueType, const std::vector<ICommandBufferPtr>& Commands) override final
	{
		Queue(QueueType)->Submit(Commands);
	}

	void SubmitCommandBuffer(EQueueType QueueType, ICommandBufferPtr& Command) override final
	{
		Queue(QueueType)->Submit(std::vector<ICommandBufferPtr>{ Command });
	}

	void SubmitCommandBuffers(const std::vector<ICommandBufferPtr>& Commands) override final;

	void SubmitCommandBuffer(ICommandBufferPtr& Command) override final;

	VkPipelineCache PipelineCache() const
	{
		return m_PipelineCache->Get();
	}

	struct VkDeviceOptions
	{
		bool8_t SubmitBarriersIM = true;
		bool8_t BatchResourceSubmit = false;
		ESyncType SyncType = ESyncType::SyncByFence;
	};

	const VkDeviceOptions& Options() const
	{
		return m_Options;
	}
protected:
private:
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkInstance m_Instance = VK_NULL_HANDLE;
	VkPhysicalDeviceLimits m_PhysicalLimits;
	VkPhysicalDeviceSparseProperties m_PhysicalDeviceSparseProperties;
	VkPhysicalDeviceFeatures m_Features;
	std::unique_ptr<VulkanPipelineCache> m_PipelineCache;
	std::array<std::unique_ptr<VulkanQueue>, static_cast<size_t>(EQueueType::Compute) + 1ull> m_Queues;
	std::pair<std::mutex, std::queue<std::shared_ptr<VulkanFence>>> m_FenceCache;
	std::vector<std::unique_ptr<VulkanDescriptorPool>> m_DescriptorPools;
	VulkanExtensions m_Extensions;
	VkDeviceOptions m_Options;
};

NAMESPACE_END(RHI)
