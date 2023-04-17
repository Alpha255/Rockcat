- Bloom
  - Offscreen RenderPass
    - FrameBuffer - Scene
      - PipelineLayout - Scene
      - DescriptorSets - Scene
        - VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, Binding::0
        - VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, Binding::1
        - VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, Binding::2
      - Pipeline - Glow
      - Shader Stages
        - VertexShader: ColorPass.vert
        - FragmentShader: ColorPass.frag
      - Draw Model UFO-Glow
      - Output: OffscreenFrameBuffer_Scene
    - FrameBuffer - Blur
      - Input: OffscreenFrameBuffer_Scene
      - PipelineLayout - Blur
      - DescriptorSets - VerticalBlur
        - VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, Binding::0
        - VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, Binding::1
      - Pipeline - VerticalBlur
        - *VkSpecializationInfo.pData = 0
      - Shader Stages
        - VertexShader: GaussBlur.vert (Fullscreen Triangle)
        - FragmentShader: GaussBlur.frag
        ```cpp
        [[vk::constant_id(0)]] const int BlurDirection = 0;
        float4 main(float2 UV : TEXCOORD0) : SV_TARGET
        {
            float Weight[5]
            {
                0.227027,
                0.1945946,
                0.1216216,
                0.054054,
                0.016216
            };
            float2 ImageSize;
            ColorImage.GetDimensions(ImageSize.x, ImageSize.y);
            float2 UVOffset = 1.0 / ImageSize * BlurSize;
            float3 OutColor = ColorImage.Sample(Sampler, UV).rgb * Weight[0];
            for (int i = 0; i < 5; ++i)
            {
                if (BlurDirection == 1)
                {
                    OutColor += ColorImage.Sample(Sampler, UV + float2(UVOffset.x * i, 0.0)).rgb * Weight[i] * BlurStrength;
                    OutColor += ColorImage.Sample(Sampler, UV - float2(UVOffset.x * i, 0.0)).rgb * Weight[i] * BlurStrength;
                }
                else
                {
                    OutColor += ColorImage.Sample(Sampler, UV + float2(0.0, UVOffset.x * i)).rgb * Weight[i] * BlurStrength;
                    OutColor += ColorImage.Sample(Sampler, UV - float2(0.0, UVOffset.x * i)).rgb * Weight[i] * BlurStrength;
                }
            }

            return float4(OutColor, 1.0f);
        }
        ```
      - Draw Fullscreen Triangle
      - Output: OffscreenFrameBuffer_VerticalBlur
  - Opaque Render Pass
    - Swapchain FrameBuffer
      - PipelineLayout - Scene
      - DescriptorSets - Scene
      - Pipeline - Phong Lighting
      - Draw Model UFO
      - Output: SwapchainFrameBuffer
      - Input: OffscreenFrameBuffer_VerticalBlur
      - PipelineLayout - Blur
      - DescriptorSets - HorizontalBlur
      - Pipeline - HorizontalBlur
      - Draw Fullscreen Triangle
      - Output: SwapchainFrameBuffer