#pragma once

#include "Asset/SerializableAsset.h"
#include "RHI/RHIResource.h"

struct VulkanExtensionSettings
{
	bool KhronosValidationLayer = true;
	bool KHRSurface = true;
	bool DebugUtils = true;
	bool DebugReport = true;
	bool ValidationFeatures = true;
	bool ValidationFeatures_GPUAssisted = false;
	bool ValidationFeatures_GPUAssistedReserveBindingSlot = false;
	bool ValidationFeatures_BestPractices = true;
	bool ValidationFeatures_DebugPrintf = false;
	bool ValidationFeatures_Synchronization = true;
	bool GetPhysicalDeviceProperties2 = false;
	bool DebugMarker = true;
	bool TimelineSemaphore = false;
	bool FullscreenExclusive = false;
	bool DynamicState = false;
	bool DepthStencilResolve = false;
	bool RenderPass2 = false;
	bool HostImageCopy = false;
	bool DynamicRendering = false;
	bool GraphicsPipelineLibrary = false;
	bool InlineUniformBlock = false;
	bool ConservativeRasterization = false;
	bool ConditionalRendering = false;
	bool DebugPrintf = false;
	bool PushDescriptor = false;
	bool Maintenance1 = false;
	bool Maintenance2 = false;
	bool Maintenance3 = false;
	bool Maintenance4 = false;
	bool Maintenance5 = false;
	bool Maintenance6 = false;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(KhronosValidationLayer),
			CEREAL_NVP(KHRSurface),
			CEREAL_NVP(DebugUtils),
			CEREAL_NVP(DebugReport),
			CEREAL_NVP(ValidationFeatures),
			CEREAL_NVP(ValidationFeatures_GPUAssisted),
			CEREAL_NVP(ValidationFeatures_GPUAssistedReserveBindingSlot),
			CEREAL_NVP(ValidationFeatures_BestPractices),
			CEREAL_NVP(ValidationFeatures_DebugPrintf),
			CEREAL_NVP(ValidationFeatures_Synchronization),
			CEREAL_NVP(GetPhysicalDeviceProperties2),
			CEREAL_NVP(DebugMarker),
			CEREAL_NVP(TimelineSemaphore),
			CEREAL_NVP(FullscreenExclusive),
			CEREAL_NVP(DynamicState),
			CEREAL_NVP(DepthStencilResolve),
			CEREAL_NVP(RenderPass2),
			CEREAL_NVP(HostImageCopy),
			CEREAL_NVP(DynamicRendering),
			CEREAL_NVP(GraphicsPipelineLibrary),
			CEREAL_NVP(InlineUniformBlock),
			CEREAL_NVP(ConservativeRasterization),
			CEREAL_NVP(ConditionalRendering),
			CEREAL_NVP(DebugPrintf),
			CEREAL_NVP(PushDescriptor),
			CEREAL_NVP(Maintenance1),
			CEREAL_NVP(Maintenance2),
			CEREAL_NVP(Maintenance3),
			CEREAL_NVP(Maintenance4),
			CEREAL_NVP(Maintenance5),
			CEREAL_NVP(Maintenance6)
		);
	}
};

struct VulkanDescriptorLimits
{
	uint32_t MaxDescriptorSetsPerPool;
	uint32_t MaxSampler;
	uint32_t MaxCombinedImageSampler;
	uint32_t MaxSampledImage;
	uint32_t MaxStorageImage;
	uint32_t MaxUniformTexelBuffer;
	uint32_t MaxStorageTexelBuffer;
	uint32_t MaxUniformBuffer;
	uint32_t MaxStorageBuffer;
	uint32_t MaxUniformBufferDynamic;
	uint32_t MaxStorageBufferDynamic;
	uint32_t MaxInputAttachment;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(MaxDescriptorSetsPerPool),
			CEREAL_NVP(MaxSampler),
			CEREAL_NVP(MaxCombinedImageSampler),
			CEREAL_NVP(MaxSampledImage),
			CEREAL_NVP(MaxStorageImage),
			CEREAL_NVP(MaxUniformTexelBuffer),
			CEREAL_NVP(MaxStorageTexelBuffer),
			CEREAL_NVP(MaxUniformBuffer),
			CEREAL_NVP(MaxStorageBuffer),
			CEREAL_NVP(MaxUniformBufferDynamic),
			CEREAL_NVP(MaxStorageBufferDynamic),
			CEREAL_NVP(MaxInputAttachment)
		);
	}
};

struct VulkanDevelopSettings : public SerializableAsset<VulkanDevelopSettings>
{
	using BaseClass::BaseClass;

	ERHIDebugLayerLevel DebugLayerLevel = ERHIDebugLayerLevel::Error;

	VulkanExtensionSettings ExtensionSettings;
	VulkanDescriptorLimits DescriptorLimits;

	uint32_t GetMaxNumDescriptor(vk::DescriptorType DescriptorType) const
	{
		switch (DescriptorType)
		{
		case vk::DescriptorType::eSampler: return DescriptorLimits.MaxSampler;
		case vk::DescriptorType::eCombinedImageSampler: return DescriptorLimits.MaxCombinedImageSampler;
		case vk::DescriptorType::eSampledImage: return DescriptorLimits.MaxSampledImage;
		case vk::DescriptorType::eStorageImage: return DescriptorLimits.MaxStorageImage;
		case vk::DescriptorType::eUniformTexelBuffer: return DescriptorLimits.MaxUniformTexelBuffer;
		case vk::DescriptorType::eStorageTexelBuffer: return DescriptorLimits.MaxStorageTexelBuffer;
		case vk::DescriptorType::eUniformBuffer: return DescriptorLimits.MaxUniformBuffer;
		case vk::DescriptorType::eStorageBuffer: return DescriptorLimits.MaxStorageBuffer;
		case vk::DescriptorType::eUniformBufferDynamic: return DescriptorLimits.MaxUniformBufferDynamic;
		case vk::DescriptorType::eStorageBufferDynamic: return DescriptorLimits.MaxStorageBufferDynamic;
		case vk::DescriptorType::eInputAttachment: return DescriptorLimits.MaxInputAttachment;
		default:
			return 0u; // Invalid descriptor type
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(DebugLayerLevel),
			CEREAL_NVP(ExtensionSettings),
			CEREAL_NVP(DescriptorLimits)
		);
	}
};
