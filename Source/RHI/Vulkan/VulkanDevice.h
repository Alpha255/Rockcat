#pragma once

#include "RHI/Vulkan/VulkanTypes.h"
#include "Runtime/Engine/RHI/RHIDevice.h"

#if 0
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
#endif

class VulkanDevice final : public RHIDevice
{
public:
	VulkanDevice(struct VulkanLayerExtensionConfigurations* Configs);

	~VulkanDevice();

	virtual void WaitIdle() const override final { assert(m_LogicalDevice); m_LogicalDevice.waitIdle(); }

	virtual RHIShaderPtr CreateShader(const RHIShaderCreateInfo& RHICreateInfo) override final;

	virtual RHIImagePtr CreateImage(const RHIImageCreateInfo& RHICreateInfo) override final;

	virtual RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo) override final;

	virtual RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo) override final;

	virtual RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) override final;

	virtual RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& RHICreateInfo) override final;

	virtual RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& RHICreateInfo) override final;

	virtual RHICommandBufferPoolPtr CreateCommandBufferPool(ERHIDeviceQueue QueueType) override final;

	virtual void SubmitCommandBuffer(ERHIDeviceQueue QueueType, RHICommandBuffer* Command) override final;

	virtual void SubmitCommandBuffer(RHICommandBuffer* Command) override final;

	virtual RHICommandBufferPtr GetActiveCommandBuffer(ERHIDeviceQueue QueueType, ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary) override final;

	inline const vk::Device& GetNative() const { return m_LogicalDevice; }

	inline const vk::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }

	const vk::Instance& GetInstance() const;

	const vk::PhysicalDeviceLimits& GetPhysicalDeviceLimits() const { return m_Limits; }
private:
	bool8_t GetQueueFamilyIndex(const vk::PhysicalDevice& PhysicalDevice, uint32_t& GraphicsQueueIndex, uint32_t& ComputeQueueIndex, uint32_t& TransferQueueIndex, uint32_t& PresentQueueIndex) const;

	std::unique_ptr<class VulkanInstance> m_Instance;

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
#endif
};
