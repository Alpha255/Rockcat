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
			CEREAL_NVP(HasRenderPass2)
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

	void SetSupported(bool Supported) { m_Supported = Supported; }
	void SetEnabledInConfig(bool Enabled) { if (m_EnabledInConfig) { *m_EnabledInConfig = Enabled; } }
private:
	std::string_view m_Name;
	bool m_Supported = false;
	bool m_Needed = false;

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