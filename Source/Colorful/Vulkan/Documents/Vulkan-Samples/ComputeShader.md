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