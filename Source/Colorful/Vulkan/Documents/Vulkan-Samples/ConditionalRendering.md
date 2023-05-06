- Instead of having to rebuild command buffers if the visibility of objects change, it's now to possible to just change a single buffer value to control if the rendering commands for that object are executed without the need to touch any command buffer.
- Enable device extension: **VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME**
- Initialize conditional buffer
    - Buffer type: **VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT**
    - Buffer format is fixed to consecutive **32-bit values**
    - Buffer offset is also aligned at **32-bit**
- RenderSceneNode
```cpp
VkConditionalRenderingBeginInfoEXT ConditionalRenderingBeginInfo{};
ConditionalRenderingBeginInfo.sType = VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_IFNO_EXT;
ConditionalRenderingBeginInfo.buffer = ConditionalBuffer;
ConditionalRenderingBeginInfo.offset = sizeof(int32_t) * Node.Index;
vkCmdBeginConditionalRenderingEXT(CommandBuffer, &ConditionalRenderingBeginInfo);
vkCmdDrawIndexed(CommandBuffer, IndexCount, 1, FirstIndex, 0, 0);
vkCmdEndConditionalRenderEXT(CommandBuffer);
```