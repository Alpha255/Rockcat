- Instancing
  - Create Instance Buffer
    - VkBufferUsageFlags::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT::VK_BUFFER_USAGE_TRANSFER_DST_BIT
  - VkPipelineVertexInputStateCreateInfo
    - std::vector<VkVertexInputBindingDescription>
      - [0] 
        - VkVertexInputBindingDescription::binding = 0
        - VkVertexInputBindingDescription::stride = sizeof(Vertex)
        - VkVertexInputBindingDescription::inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX
      - [1]
        - VkVertexInputBindingDescription::binding = 1
        - VkVertexInputBindingDescription::stride = sizeof(InstanceData)
        - VkVertexInputBindingDescription::inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE
    - std::vector<VkVertexInputAttributeDescription>
      - Binding::0, Location::0-3
      - Binding::1, Location::4-7
  - Swapchain FrameBuffer
    - DescriptorSets - InstanceRocks
    - Pipeline - InstancedRocks
      - VertexShader: Instancing.vert
      ```cpp
      struct VSInput
      {
          [[vk::location(0)]] float3 Pos : POSITION0;
          [[vk::location(1)]] float3 Normal : NORMAL;
          [[vk::location(2)]] float3 Color : COLOR;
          [[vk::location(3)]] float2 UV : TEXCOORD0;

          [[vk::location(4)]] float3 InstancedPos : POSITION1;
          [[vk::location(5)]] float3 InstancedRot : TEXCOORD1;
          [[vk::location(6)]] float3 InstancedScale : TEXCOORD2;
          [[vk::location(7)]] int InstancedTexIndex : TEXCOORD3;
      }
      VSOutput main(VSInput Input)
      {
          ...
          float Sin = sin(Input.InstancedRot.x + UBO.LocSpeed);
          float Cos = cos(Input.InstancedRot.x + UBO.LocSpeed);
          ...
      }
      ```
    - vkCmdDrawIndexed(CommandBuffer, IndexCount, InstanceCount, 0, 0, 0)