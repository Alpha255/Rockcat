#pragma once

#include "RHI/Vulkan/VulkanTypes.h"
#include "Engine/RHI/RHIDevice.h"

#if 0
struct VulkanExtensions
{
	bool Synchronization2 = false;
	bool ExtendedDynamicState = false;
	bool ExtendedDynamicState2 = false;
	bool TimelineSemaphore = false;
	bool RenderPass2 = false;
	bool FullscreenExclusive = false;
	bool DebugUtils = false;
	bool DebugReport = false;
	bool DebugMarker = false;
	bool Maintenance1 = false;
};

enum class ESyncType
{
	SyncByFence,
	QueueWaitSemaphore, /// Can't insure the texture inuse is complete upload when not use async transfer ???
	ForceWaitQueueIdle  /// Can't insure the texture inuse is complete upload when not use async transfer ???
};
#endif

class VulkanDevice final : public RHIDevice
{
public:
	VulkanDevice(struct VulkanLayerExtensionConfigurations* Configs);

	~VulkanDevice();

	virtual void WaitIdle() const override final { assert(m_LogicalDevice); m_LogicalDevice.waitIdle(); }

	virtual RHIShaderPtr CreateShader(const RHIShaderCreateInfo& CreateInfo) override final;
	virtual RHIImagePtr CreateImage(const RHIImageCreateInfo& CreateInfo) override final;
	virtual RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& CreateInfo) override final;
	virtual RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& CreateInfo) override final;
	virtual RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo) override final;
	virtual RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& CreateInfo) override final;
	virtual RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& CreateInfo) override final;
	virtual void SubmitCommandBuffer(RHICommandBuffer* Command) override final;
	virtual RHICommandBuffer* GetActiveCommandBuffer(ERHIDeviceQueue QueueType, ERHICommandBufferLevel Level) override final;

	inline const vk::Device& GetNative() const { return m_LogicalDevice; }
	inline const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
	const vk::Instance& GetInstance() const;
	const vk::PhysicalDeviceLimits& GetPhysicalDeviceLimits() const { return m_Limits; }
private:
	bool GetQueueFamilyIndex(const vk::PhysicalDevice& PhysicalDevice, uint32_t& GraphicsQueueIndex, uint32_t& ComputeQueueIndex, uint32_t& TransferQueueIndex, uint32_t& PresentQueueIndex) const;

	std::unique_ptr<class VulkanInstance> m_Instance;
	std::unique_ptr<class VulkanCommandBufferManager> m_TransferCmdMgr;
	std::unique_ptr<class VulkanCommandBufferManager> m_ComputeCmdMgr;
	std::queue<std::shared_ptr<class VulkanCommandBufferManager>> m_GraphicsCmdMgrs;
	std::array<std::unique_ptr<class VulkanQueue>, (size_t)ERHIDeviceQueue::Num> m_Queues;

	vk::PhysicalDeviceLimits m_Limits;

	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_LogicalDevice;
#if 0
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

	VkPipelineCache PipelineCache() const
	{
		return m_PipelineCache->Get();
	}

	struct VkDeviceOptions
	{
		bool SubmitBarriersIM = true;
		bool BatchResourceSubmit = false;
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
#endif
};
