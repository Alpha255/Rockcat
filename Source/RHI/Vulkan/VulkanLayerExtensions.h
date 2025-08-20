#pragma once

#include "RHI/Vulkan/VulkanTypes.h"
#include "RHI/Vulkan/VulkanDevelopSettings.h"

#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

using VulkanLayerArray = std::vector<std::unique_ptr<class VulkanLayer>>;
using VulkanExtensionArray = std::vector<std::unique_ptr<class VulkanExtension>>;

template<class LastStruct, class NextStruct>
inline void SetPNext(LastStruct& Last, NextStruct& Next)
{
	Next.pNext = (void*)Last.pNext;
	Last.pNext = (void*)&Next;
}

class VulkanLayer
{
public:
	using OnInstanceCreation = std::function<void(const VulkanExtensionSettings&, vk::InstanceCreateInfo&)>;
	using OnDeviceCreation = std::function<void(const VulkanExtensionSettings&, vk::DeviceCreateInfo&)>;

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

	static VulkanLayerArray GetWantedInstanceLayers(VulkanExtensionSettings& Configs);
	static VulkanLayerArray GetWantedDeviceLayers(VulkanExtensionSettings& Configs);
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

	static VulkanExtensionArray GetWantedInstanceExtensions(VulkanExtensionSettings& Configs);
	static VulkanExtensionArray GetWantedDeviceExtensions(VulkanExtensionSettings& Configs);
};

void LogEnabledLayerAndExtensions(const VulkanLayerArray& Layers, const VulkanExtensionArray& Extensions, const char* Category);