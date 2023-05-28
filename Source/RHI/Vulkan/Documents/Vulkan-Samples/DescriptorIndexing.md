- Creating instance and device
    - Enable instance extension:            **VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME**
    - Enable device extension: 
        - **VK_KHR_MAINTENANCE3_EXTENSION_NAME**
        - **VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME**
    - **VkPhysicalDeviceDescriptorIndexingFeaturesEXT**:
        - .shaderSampledImageArrayNonUniformIndexing = VK_TRUE
        - .runtimeDescriptorArray = VK_TRUE
        - .descriptorBindingVariableDescriptorCount = VK_TRUE
    - **VkDeviceCreateInfo**::**pNext** = **&VkPhysicalDeviceDescriptorIndexingFeaturesEXT**
- Allocate descriptor sets
    - **VkDescriptorSetLayoutBindingFlagsCreateInfoEXT**
        - .bindingCount = 2
        - .pBindingFlags = {**VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT**}
    - **VkDescriptorSetLayoutCreateInfo**::**pNext** = **&VkDescriptorSetLayoutBindingFlagsCreateInfoEXT**
    - **VkDescriptorSetVariableDescriptorCountAllocateInfoEXT**:
        - .descriptorSetCount = 1
        - .pDescriptorCounts = {Textures.size()}
- Fragment Shader
```cpp
Texture2D Textures[];
SamplerState ColorMapSampler;

struct VSOutput
{
    float4 Pos : SV_POSITION;
    [[vk::location(0)]] int TextureIndex : TEXTUREINDEX0;
    [[vk::location(1)]] float2 UV : TEXCOORD0;
};

float4 main(VSOutput Input) : SV_TARGET
{
    return Textures[NonUniformResourceIndex(Input.TextureIndex)].Sample(ColorMapSampler, Input.UV);
}
```