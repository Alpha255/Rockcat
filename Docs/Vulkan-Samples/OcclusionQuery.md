- OcclusionQuery
  - Create Query Pool
    - VkQueryPoolCreateInfo::queryType = VK_QUERY_TYPE_OCCLUSION
  - Draw
    - Get Query Result
      - vkGetQueryPoolResults(Device, QueryPool, FirstQuery, QueryCount, DataSize, pData, Stride, VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT)
    - Bind Simple Pipeline, Draw To Swapchain FrameBuffer
      - vkCmdResetQueryPool (Must Out of render pass)
      - Draw Plane As Occluder
      - vkCmdBeginQuery(CommandBuffer, QueryPool, QueryIndex, VK_FLAGS_NONE)
        - Draw Teapot
        - vkCmdEndQuery(CommandBuffer, QueryPool, QueryIndex)
      - vkCmdBeginQuery(CommandBuffer, QueryPool, QueryIndex, VK_FLAGS_NONE)
        - Draw Sphere
        - vkCmdEndQuery(CommandBuffer, QueryPool, QueryIndex)
    - Update Uniform Buffers
      - UBO.Visible = QueryPassed ? 1.0 : 0.0
    - Clear Color And Depth Attachments
    - Bind Solid Pipeline
      - Draw Teapot
      - Draw Sphere
      ```cpp
      /// Fragment shader
      struct VSOutput
      {
          [[vk::location(0)]] float3 Normal : NORMAL0;
          [[vk::location(1)]] float3 Color : COLOR0;
          [[vk::location(2)]] float Visible : TEXCOORD3;
          ...
      };
      float4 main(VSOutput Input) : SV_TARGET
      {
          if (Input.Visible > 0.0f)
          {
              /// Lighting Color
          }
          else
          {
              return float4(0.1, 0.1, 0.1, 1.0);
          }
      }
      ```
    - Bind Occluder Pipeline
      - Draw Plane