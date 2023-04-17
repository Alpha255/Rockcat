- IndirectDraw
  - Create Indirect Buffer
  ```cpp
  uint32_t Index = 0;
  uint32_t OjectCount = 0;
  std::vector<VkDrawIndexedIndirectCommand> IndirectCmds;
  for (auto& Node : Models)
  {
      VkDrawIndexedIndirectCommand IndirectCmd;
      IndirectCmd.instanceCount = OBJECT_INSTANCE_COUNT;
      IndirectCmd.firstInstance = Index * OBJECT_INSTANCE_COUNT;
      IndirectCmd.firstIndex = Node.MeshPrimitive.FirstIndex;
      IndirectCmd.indexCount = Node.MeshPrimitive.IndexCount;
      IndirectCmds.emplace_back(IndirectCmd);
      OjectCount += instanceCount;
      ++Index;
  }
  ...
  // Create and copy buffer to IndirectBuffer
  ```
  - Create Instance Buffer
  ```cpp
  for (uint32_t i = 0; i < ObjectCount; ++i)
  {
      // RandomRot
      // RandomPos
      // RandomScale
      // SetImageIndex
  }
  ...
  // Create and copy buffer to InstanceBuffer
  ```
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
  - Draw
    - vkCmdBindPipeline
    - vkCmdBindVertexBuffers(CommandBuffer, VERTEX_BUFFER_ID, 1, VerticesBuffer)
    - vkCmdBindVertexBuffers(CommandBuffer, INSTANCE_BUFFER_ID, 1, InstanceBuffer)
    - vkCmdBindIndexBuffer(CommandBuffer, IndicesBuffer)
    ```cpp
    if (VulkanDevice.SupportMultiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(CommandBuffer, IndirectBuffer, 0, IndirectCmds.size(), sizeof(VkDrawIndexedIndirectCommand))
    }
    else
    {
        for (uint32_t i = 0; i < IndirectCmds.size(); ++i)
        {
            vkCmdDrawIndexedIndirect(CommandBuffer, IndirectBuffer, i * sizeof(VkDrawIndexedIndirectCommand), 1, sizeof(VkDrawIndexedIndirectCommand))
        }
    }
    /// TODO: Multi primitives
    ```