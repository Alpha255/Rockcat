#pragma once

#include "RHI/Vulkan/VulkanTypes.h"
#include "Engine/Asset/SerializableAsset.h"

#define VK_LAYER_EXT_CONFIG_PATH "Configs\\VkLayerAndExtensionConfigs.json"
#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

using VulkanLayerArray = std::vector<std::unique_ptr<class VulkanLayer>>;
using VulkanExtensionArray = std::vector<std::unique_ptr<class VulkanExtension>>;

template<class LastStruct, class NextStruct>
inline void SetPNext(LastStruct& Last, NextStruct& Next)
{
	Next.pNext = (void*)Last.pNext;
	Last.pNext = (void*)&Next;
}

struct VulkanLayerExtensionConfigurations : public SerializableAsset<VulkanLayerExtensionConfigurations>
{
	using BaseClass::BaseClass;

	ERHIDebugLayerLevel DebugLayerLevel = ERHIDebugLayerLevel::Error;

	bool HasKhronosValidationLayer = true;

	bool HasKHRSurface = true;
	bool HasDebugUtils = true;
	bool HasDebugReport = true;
	bool HasValidationFeatures = true;
	bool HasValidationFeatures_GPUAssisted = false;
	bool HasValidationFeatures_GPUAssistedReserveBindingSlot = false;
	bool HasValidationFeatures_BestPractices = true;
	bool HasValidationFeatures_DebugPrintf = false;
	bool HasValidationFeatures_Synchronization = true;
	bool HasGetPhysicalDeviceProperties2 = false;
	bool HasDebugMarker = true;
	bool HasTimelineSemaphore = false;
	bool HasFullscreenExclusive = false;
	bool HasDynamicState = false;
	bool HasDepthStencilResolve = false;
	bool HasRenderPass2 = false;
	bool HasHostImageCopy = false;
	bool HasDynamicRendering = false;
	bool HasGraphicsPipelineLibrary = false;
	bool HasInlineUniformBlock = false;
	bool HasConservativeRasterization = false;
	bool HasConditionalRendering = false;
	bool HasDebugPrintf = false;
	bool HasPushDescriptor = false;
	bool HasMaintenance1 = false;
	bool HasMaintenance2 = false;
	bool HasMaintenance3 = false;
	bool HasMaintenance4 = false;
	bool HasMaintenance5 = false;
	bool HasMaintenance6 = false;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(DebugLayerLevel),
			CEREAL_NVP(HasKhronosValidationLayer),
			CEREAL_NVP(HasKHRSurface),
			CEREAL_NVP(HasDebugUtils),
			CEREAL_NVP(HasDebugReport),
			CEREAL_NVP(HasValidationFeatures),
			CEREAL_NVP(HasValidationFeatures_GPUAssisted),
			CEREAL_NVP(HasValidationFeatures_GPUAssistedReserveBindingSlot),
			CEREAL_NVP(HasValidationFeatures_BestPractices),
			CEREAL_NVP(HasValidationFeatures_DebugPrintf),
			CEREAL_NVP(HasValidationFeatures_Synchronization),
			CEREAL_NVP(HasGetPhysicalDeviceProperties2),
			CEREAL_NVP(HasDebugMarker),
			CEREAL_NVP(HasTimelineSemaphore),
			CEREAL_NVP(HasFullscreenExclusive),
			CEREAL_NVP(HasDynamicState),
			CEREAL_NVP(HasDepthStencilResolve),
			CEREAL_NVP(HasRenderPass2),
			CEREAL_NVP(HasHostImageCopy),
			CEREAL_NVP(HasDynamicRendering),
			CEREAL_NVP(HasGraphicsPipelineLibrary),
			CEREAL_NVP(HasInlineUniformBlock),
			CEREAL_NVP(HasConservativeRasterization),
			CEREAL_NVP(HasConditionalRendering),
			CEREAL_NVP(HasDebugPrintf),
			CEREAL_NVP(HasPushDescriptor),
			CEREAL_NVP(HasMaintenance1),
			CEREAL_NVP(HasMaintenance2),
			CEREAL_NVP(HasMaintenance3),
			CEREAL_NVP(HasMaintenance4),
			CEREAL_NVP(HasMaintenance5),
			CEREAL_NVP(HasMaintenance6)
		);
	}
};

class VulkanLayer
{
public:
	using OnInstanceCreation = std::function<void(const VulkanLayerExtensionConfigurations&, vk::InstanceCreateInfo&)>;
	using OnDeviceCreation = std::function<void(const VulkanLayerExtensionConfigurations&, vk::DeviceCreateInfo&)>;

	VulkanLayer(const char* Name, bool Supported, bool Needed, bool* EnabledInConfig)
		: m_Name(Name)
		, m_Supported(Supported)
		, m_Needed(Needed)
		, m_EnabledInConfig(EnabledInConfig)
	{
	}

	inline bool IsEnabled() const { return m_EnabledInConfig ? (*m_EnabledInConfig && m_Supported) : m_Supported; }
	inline bool IsSupported() const { return m_Supported; }
	inline bool IsNeeded() const { return m_Needed; }
	inline uint32_t GetSpecVersion() const { return m_SpecVersion; }
	inline const char* GetName() const { return m_Name.data(); }
	inline OnInstanceCreation& GetOnInstanceCreation() { return m_OnInstanceCreation; }
	inline OnDeviceCreation& GetOnDeviceCreation() { return m_OnDeviceCreation; }

	inline void SetOnInstanceCreation(OnInstanceCreation&& Func) { m_OnInstanceCreation = std::move(Func); }
	inline void SetOnDeviceCreation(OnDeviceCreation&& Func) { m_OnDeviceCreation = std::move(Func); }

	static VulkanLayerArray GetWantedInstanceLayers(VulkanLayerExtensionConfigurations& Configs);
	static VulkanLayerArray GetWantedDeviceLayers(VulkanLayerExtensionConfigurations& Configs);
protected:
	friend class VulkanInstance;
	friend class VulkanDevice;

	void SetEnabledInConfig(bool Enabled) { if (m_EnabledInConfig) { *m_EnabledInConfig = Enabled; } }
	void SetSpecVersion(uint32_t Version) { m_SpecVersion = Version; }
private:
	std::string_view m_Name;
	bool m_Supported = false;
	bool m_Needed = false;
	uint32_t m_SpecVersion = 0u;
	bool* m_EnabledInConfig = nullptr;
	OnInstanceCreation m_OnInstanceCreation;
	OnDeviceCreation m_OnDeviceCreation;
};

class VulkanExtension : public VulkanLayer
{
public:
	using VulkanLayer::VulkanLayer;

	static VulkanExtensionArray GetWantedInstanceExtensions(VulkanLayerExtensionConfigurations& Configs);
	static VulkanExtensionArray GetWantedDeviceExtensions(VulkanLayerExtensionConfigurations& Configs);
};

void LogEnabledLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char* Category);