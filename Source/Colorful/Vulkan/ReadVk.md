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
- Multisampling
  - SampleCount = min(VkDeviceProperties.limits.framebufferColorSampleCounts, VkDeviceProperties.limits.framebufferDepthSampleCounts)
  - Create Graphics Pipeline
    - VkPipelineMultisampleStateCreateInfo::rasterizationSamples = SampleCount
      - Optional: VkPipelineMultisampleStateCreateInfo::sampleShadingEnable
  - Create Multisampled Image/ImageView
    - Color
      - VkImageCreateInfo::samples = SampleCount
      - VkMemoryPropertyFlags::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT (Fallback to  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT if not supported)
    - Depth
      - VkImageCreateInfo::samples = SampleCount
      - VkMemoryPropertyFlags::VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT (Fallback to  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT if not supported)
  - Create FrameBuffers
    - [0] Multisampled Color ImageView
    - [1] Swapchain ImageView
    - [2] Multisampled Depth ImageView
  - Create RenderPass
    - Attachments
      - [0]: VkAttachmentDescription::samples = SampleCount (Multisampled render target)
      - [1]: VkAttachmentDescription::samples = VK_SAMPLE_COUNT_1_BIT (Resolved render target)
      - [2]: VkAttachmentDescription::samples = VK_SAMPLE_COUNT_1_BIT (Multisampled depth)
    - SubpassDependencies
      - [0]: srcStageMask::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, dstStageMask::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, srcAccessMask::VK_ACCESS_MEMORY_READ_BIT, dstAccessMask::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
      - [0]: srcStageMask::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, dstStageMask::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, srcAccessMask::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, dstAccessMask::VK_ACCESS_MEMORY_READ_BIT
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
- ComputeShader
  - BuildComputeCommandBuffer
    - vkBeginCommandBuffer
    - vkCmdBindPipeline(Compute.CmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE)
    - vkCmdBindDescriptorSets(Compute.CmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE)
    - vkCmdDispatch(Compute.CmdBuffer, ComputeTexTarget.Width / 16, ComputeTexTarget.Height / 16)
    - vkEndCommandBuffer
  - BuildCommandBuffers
    - vkBeginCommandBuffer
    - VkImageMemoryBarrier
      - oldLayout = VK_IMAGE_LAYOUT_GENERAL
      - newLayout = VK_IMAGE_LAYOUT_GENERAL
      - srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT
      - dstAccessMask = VK_ACCESS_SHADER_READ_BIT
    - vkCmdPipelineBarrier(DrawCmdBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
    - vkCmdBeginRenderPass(DrawCmdBuffer)
    - vkCmdSetViewport(DrawCmdBuffer, 0, 1, VkViewport{0.0f, 0.0f, Width * 0.5f, Height, 0.0f, 1.0f})
    - DrawQuad
    - vkCmdSetViewport(DrawCmdBuffer, 0, 1, VkViewport{Width * 0.5f, 0.0f, Width * 0.5f, Height, 0.0f, 1.0f})
    - DrawQuad
  - VkSubmitInfo SubmitCompute
    - commandBufferCount = 1
    - pCommandBuffers = &Compute.CmdBuffer
    - waitSemaphoreCount = 1
    - pWaitSemaphores = &Graphics.Semaphore
    - pWaitDstStageMask = [VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT]
    - signalSemaphoreCount = 1
    - pSignalSemaphores = &Compute.Semaphore
  - VkSubmitInfo SubmitGraphics
    - commandBufferCount = 1
    - pCommandBuffers = &DrawCmdBuffer
    - waitSemaphoreCount = 2
    - pWaitSemaphores = [Compute.Semaphore, PresentComplete]
    - pWaitDstStageMask = [VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT]
    - signalSemaphoreCount = 2
    -  pSignalSemaphores = [Graphics.Semaphore, RenderComplete]
  - Emboss
    ```cpp
    // Copyright 2020 Google LLC

    Texture2D inputImage : register(t0);
    RWTexture2D<float4> resultImage : register(u1);

    float conv(in float kernel[9], in float data[9], in float denom, in float offset)
    {
        float res = 0.0;
        for (int i=0; i<9; ++i)
        {
            res += kernel[i] * data[i];
        }
        return saturate(res/denom + offset);
    }

    [numthreads(16, 16, 1)]
    void main(uint3 GlobalInvocationID : SV_DispatchThreadID)
    {
	      float imageData[9];
	      // Fetch neighbouring texels
	      int n = -1;
	      for (int i=-1; i<2; ++i)
	      {
		        for(int j=-1; j<2; ++j)
		        {
			          n++;
			          float3 rgb = inputImage[uint2(GlobalInvocationID.x + i, GlobalInvocationID.y + j)].rgb;
			          imageData[n] = (rgb.r + rgb.g + rgb.b) / 3.0;
		        }
	      }

	      float kernel[9];
	      kernel[0] = -1.0; kernel[1] =  0.0; kernel[2] =  0.0;
	      kernel[3] = 0.0; kernel[4] = -1.0; kernel[5] =  0.0;
	      kernel[6] = 0.0; kernel[7] =  0.0; kernel[8] = 2.0;

	      float4 res = float4(conv(kernel, imageData, 1.0, 0.50).xxx, 1.0);

	      resultImage[int2(GlobalInvocationID.xy)] = res;
    }
    ```
- ParticleFire (CPU based particle system)
  ```cpp
  #define PARTICLE_COUNT 512
  #define PARTICLE_SIZE 10.0f
  #define FLAME_RADIUS 8.0f
  #define PARTICLE_TYPE_FLAME 0
  #define PARTICLE_TYPE_SMOKE 1

  struct Particle
  {
      vec4 Pos;
      vec4 Color;
      vec4 Vel;
      float Alpha;
      float Size;
      float Rotation;
      float RotationSpeed;
      uint32_t Type;
  };

  vec3 EmitterPos = Vec3(0.0f, -FLAME_RADIUS + 2.0f, 0.0f);
  vec3 MinVel = vec3(-3.0f, 0.5f, -3.0f);
  vec3 MaxVel = vec3(3.0f, 7.0f, 3.0f);

  std::default_random_engine RandEngine;
  RandEngine.seed((unsigned))

  float Random(float Range)
  {
      std::uniform_real_distribution<float> RandDist(0.0f, Range);
      return RandDist()
  }
  ```
  - InitParticles
  ```cpp
  void InitParticle(Particle& Particle, vec3 EmitterPos)
  {
      Particle.Vel = vec4(0.0f, MinVel.y + Random(MaxVel.y - MinVel.y), 0.0f, 0.0f);
      //Particle.Alpha = Random(0.75f);
      Particle.Size = 1.0f + Random(0.5f);
      Particle.Color = vec4(1.0f);
      Particle.Type = PARTICLE_TYPE_FLAME;
      Particle.Rotation = Random(2.0f * PI);
      Particle.RotationSpeed = Random(2.0f) - Random(2.0f);

      float Theta = Random(2.0f * PI);
      float Phi = Random(PI) - PI / 2.0f;
      float R = Random(FLAME_RADIUS);

      Particle.Pos.x = R * cos(Theta) * cos(Phi);
      Particle.Pos.y = R * sin(Theta);
      Particle.Pos.z = R * sin(Theta) * cos(Phi);
      Particle.Pos += vec4(EmitterPos, 0.0f);
  }
  ```
  - PrepareParticles
  ```cpp
  ParticleBuffers.Resize(PARTICLE_COUNT);
  for (auto& Particle : ParticleBuffers)
  {
      InitParticle(Particle, EmitterPos);
      Particle.Alpha = 1.0f - (abs(Particle.Pos.y) / FLAME_RADIUS * 2.0f);
  }
  // Create Vertex Buffer
  ```
  - UpdateParticles
  ```cpp
  float ParticleTimer = FrameTimer * 0.45f;
  for (auto& Particle : ParticleBuffer)
  {
      switch (Particle.Type)
      {
          case PARTICLE_TYPE_FLAME:
            Particle.Pos.y -= Particle.Vel.y * ParticleTimer * 3.5f;
            Particle.Alpha += ParticleTimer * 2.5f;
            Particle.Size -= ParticleTimer * 0.5f;
            break;
          case PARTICLE_TYPE_SMOKE:
            Particle.Pos -= Particle.Vel * FrameTimer;
            Particle.Alpha += ParticleTimer * 0.25f;
            Particle.Size += ParticleTimer * 0.125f;
            Particle.Color -= ParticleTimer * 0.05f;
            break;
      }
      Particle.Rotation += ParticleTimer * Particle.RotationSpeed;
      if (Particle.Alpha > 2.0f)
      {
          {
              switch (Particle.Type)
              {
                  case PARTICLE_TYPE_FLAME:
                    if (Random(1.0f) < 0.05f)
                    {
                        Particle.Alpha = 0.0f;
                        Particle.Color = vec4(0.25f + Random(0.25f));
                        Particle.Pos.x *= 0.5f;
                        Particle.Pos.z *= 0.5f;
                        Particle.Vel = vec4(Random(1.0f) - Random(1.0f), (MinVel.y * 2) + Random(MaxVel.y - MinVel.y), Random(1.0f) - Random(1.0f), 0.0f);
                        Particle.Size = 1.0f + Random(0.5f);
                        Particle.RotationSpeed = Random(1.0f) - Random(1.0f);
                        Particle.Type = PARTICLE_TYPE_SMOKE;
                    }
                    else
                    {
                        InitParticles(Particle, EmitterPos);
                    }
                    break;
                  case PARTICLE_TYPE_SMOKE:
                    InitParticles(Particle, EmitterPos);
                    break;
              }
          }
      }
  }
  ```
  - DrawParticles
    - vkCmdDraw(DrawCmdBuffer, PARTICLE_COUNT, 1, 0, 0);
    - VertexShader
    ```cpp
    struct VSOutput
    {
        float4 Pos : SV_POSITION;
        [[vk::builtin("PointSize")]] float PSize : PSIZE;
        [[vk::location(0)]] float4 Color : COLOR0;
        [[vk::location(1)]] float Alpha : TEXCOORD0;
        [[vk::location(2)]] int Type : TEXCOORD1;
        [[vk::location(3)]] float Rotation : TEXCOORD2;
        [[vk::location(4)]] float2 CenterPos : POSITION1;
        [[vk::location(5)]] float PointSize : TEXCOORD3;
    };

    struct UBO
    {
        float4x4 Projection;
        float4x4 ModelView;
        float2 ViewportDim;
        float PointSize;
    };
    cbuffer Ubo : register(b0)  
    {
        UBO Ubo;
    };

    VSOutput main(VSInput Input)
    {
        /// ...
        Output.Pos = mul(Ubo.Projection, mul(Ubo.ModelView, float4(Input.Pos.xyz, 1.0f)));

        float SpriteSize = Input.Size * 8.0f;
        float4 EyePos = mul(Ubo.ModelView, float4(Input.Pos.xyz, 1.0f));
        float4 ProjectedCorner = mul(Ubo.Projection, float4(0.5f * SpriteSize, 0.5f * SpriteSize, EyePos.z, EyePos.w));
        Output.PointSize = Output.PSize = Ubo.ViewportDim.x * ProjectedCorner.x / ProjectedCorner.w;
        Output.CenterPos = ((Output.Pos.xy / Output.Pos.w) + 1.0f) * 0.5f * Ubo.ViewportDim;
        return Output;
    }
    ```
    - PixelShader
    ```cpp
    float4 main(VSOutput Input) : SV_TARGET
    {
        float4 Color;
        float Alpha = Input.Alpha <= 1.0f ? Input.Alpha : (2.0f - Input.Alpha);

        float RotCenter = 0.5f;
        float RotCos = cos(Input.Rotation);
        float RotSin = sin(Input.Rotation);

        float2 PointCoord = (Input.Pos.xy - Input.CenterPos.xy) / Input.PointSize + 0.5f;

        float2 RotUV = float2(
          RotCos * (PointCoord.x - RotCenter) + RotSin * (PointCoord.x - RotCenter) + RotCenter,
          RotCos * (PointCoord.y - RotCenter) - RotSin * (PointCoord.y - RotCenter) + RotCenter);

        float4 OutColor;
        if (Input.Type == 0)
        {
            Color = TextureFire.Sample(SamplerFire, RotUV);
            OutColor.a = 0.0f;
        } 
        else
        {
            Color = TextureSmoke.Sample(SamplerSmoke, RotUV);
            OutColor.a = Color.a * Alpha;
        }

        OutColor.rgb = Color.rgb * Input.Color.rgb * Alpha;
        return OutColor;
    }
    ```