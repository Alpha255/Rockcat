- Dynamic UniformBuffer
  - Creation: 
    - **VkBufferUsageFlags::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT**
    - VkMemoryPropertyFlags::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
  - AllocAndWriteDescriptor:
    - Create DescriptorSet Layout
      - **VkDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC**
    - **VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC**
    - **VkDescriptorBufferInfo.Range = Alignment**
  - VertexShader:
    ```cpp
    struct UBOInstance
    {
        float4x4 ModelMatrix;
    };
    cbuffer UBO : register(b1)
    {
        UBOInstance Buffer;
    }
    VSOutput main(VSInput Input)
    {
        ...
        float4x4 ModelView = mul(ViewMatrix, Buffer.ModelMatrix);
        ...
    }
    ```
    - Update
      - memcpy(MappedMemory, SorceData, Size)
      - vkFlushMappedMemoryRanges