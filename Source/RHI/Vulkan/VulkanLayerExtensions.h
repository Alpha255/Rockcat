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

	bool HasKHRSurfaceExt = true;
	bool HasDebugUtilsExt = true;
	bool HasDebugReportExt = true;
	bool HasValidationFeaturesExt = true;
	bool HasValidationFeaturesExt_GPUAssisted = false;
	bool HasValidationFeaturesExt_GPUAssistedReserveBindingSlot = false;
	bool HasValidationFeaturesExt_BestPractices = true;
	bool HasValidationFeaturesExt_DebugPrintf = false;
	bool HasValidationFeaturesExt_Synchronization = true;
	bool HasGetPhysicalDeviceProperties2 = false;
	bool HasDebugMarkerExt = true;
	bool HasTimelineSemaphore = false;
	bool HasFullscreenExclusive = false;
	bool HasDynamicState = false;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(DebugLayerLevel),
			CEREAL_NVP(HasKhronosValidationLayer),
			CEREAL_NVP(HasKHRSurfaceExt),
			CEREAL_NVP(HasDebugUtilsExt),
			CEREAL_NVP(HasDebugReportExt),
			CEREAL_NVP(HasValidationFeaturesExt),
			CEREAL_NVP(HasValidationFeaturesExt_GPUAssisted),
			CEREAL_NVP(HasValidationFeaturesExt_GPUAssistedReserveBindingSlot),
			CEREAL_NVP(HasValidationFeaturesExt_BestPractices),
			CEREAL_NVP(HasValidationFeaturesExt_DebugPrintf),
			CEREAL_NVP(HasValidationFeaturesExt_Synchronization),
			CEREAL_NVP(HasGetPhysicalDeviceProperties2),
			CEREAL_NVP(HasDebugMarkerExt),
			CEREAL_NVP(HasTimelineSemaphore),
			CEREAL_NVP(HasFullscreenExclusive)
		);
	}
};

class VulkanLayer
{
public:
	VulkanLayer(const char* Name, bool Needed)
		: m_Name(Name)
		, m_Needed(Needed)
	{
	}

	inline bool IsEnabled() const { return m_Enabled; }
	inline bool IsNeeded() const { return m_Needed; }
	inline const char* GetName() const { return m_Name.data(); }

	static VulkanLayerArray GetWantedInstanceLayers();

	static VulkanLayerArray GetWantedDeviceLayers();
protected:
	friend class VulkanInstance;
	friend class VulkanDevice;

	bool SetEnabled(const VulkanLayerExtensionConfigurations* Configs, bool Supported) { m_Enabled = IsEnabledInConfig(Configs) && Supported; return m_Enabled; }
	virtual bool IsEnabledInConfig(const VulkanLayerExtensionConfigurations* Configs) const { return Configs && false; }
	virtual void SetEnabledToConfig(VulkanLayerExtensionConfigurations* /*Config*/) const {}
private:
	std::string_view m_Name;
	bool m_Enabled = false;
	bool m_Needed = false;
};

class VulkanExtension : public VulkanLayer
{
public:
	using VulkanLayer::VulkanLayer;

	static VulkanExtensionArray GetWantedInstanceExtensions();

	static VulkanExtensionArray GetWantedDeviceExtensions();
};

class VulkanInstanceExtension : public VulkanExtension
{
public:
	using VulkanExtension::VulkanExtension;

	virtual void PreInstanceCreation(VulkanLayerExtensionConfigurations* Configs, vk::InstanceCreateInfo& /*CreateInfo*/) { SetEnabledToConfig(Configs); }
protected:
	friend class VulkanInstance;
};

class VulkanDeviceExtension : public VulkanExtension
{
public:
	using VulkanExtension::VulkanExtension;

	virtual void PreDeviceCreation(VulkanLayerExtensionConfigurations* Configs, vk::DeviceCreateInfo& /*CreateInfo*/) { SetEnabledToConfig(Configs); }
protected:
	friend class VulkanDevice;
};

void LogEnabledLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char* Category);