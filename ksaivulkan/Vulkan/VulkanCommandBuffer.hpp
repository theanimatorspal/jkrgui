#pragma once
#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanRenderPass.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <Pipeline/VulkanPipelineLayout.hpp>
namespace ksai {

class VulkanCommandBuffer {
      public:
          enum class Type { Primary, Secondary };
          enum class BeginContext { Normal, ContinueRenderPass, OneTimeSubmit, ContinueRenderPassAndOneTimeSubmit };

          VulkanCommandBuffer(const VulkanDevice& inDevice, const VulkanCommandPool& inPool, Type inContext = Type::Primary);
          ~VulkanCommandBuffer();
          GETTER& GetCommandBufferHandle() const { return mBuffer; }
          template <BeginContext inContext = BeginContext::Normal> void Begin() const;

          template <BeginContext inContext = BeginContext::Normal>
          void Begin(VulkanRenderPassBase& inRenderPass, ksai::ui inSubpass, VulkanFrameBufferBase& inFrameBuffer) const;
          void Reset() const { mBuffer.reset(); }
          void End() const { mBuffer.end(); }
          inline void ExecuteCommands(const VulkanCommandBuffer& inBuffer) const { mBuffer.executeCommands(inBuffer.mBuffer); }

          enum class RenderPassBeginContext { Inline, SecondaryCommandBuffers };
          template <RenderPassBeginContext inBeginContext = RenderPassBeginContext::Inline>
          void BeginRenderPass(const VulkanRenderPassBase& inRenderPass,
                               const VulkanSurface& inSurface,
                               const VulkanFrameBufferBase& inFrameBuffer,
                               std::array<float, 5> inClearValue) const;
          void EndRenderPass() const;
          template <class T> void PushConstants(const VulkanPipelineLayoutBase& inPipelineLayout, const vk::ArrayProxy<const T> inValues) const {
                    mBuffer.pushConstants<T>(inPipelineLayout.GetPipelineLayoutHandle(),
                                             vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment |
                                                       vk::ShaderStageFlagBits::eCompute,
                                             0,
                                             inValues);
          }
          void SetViewport(const VulkanSurface& inSurface) {
                    const auto& Extent = inSurface.GetExtent();
                    mBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(Extent.width), static_cast<float>(Extent.height), 0.0f, 1.0f));
          }
          void SetScissor(const VulkanSurface& inSurface) {
                    const auto& Extent = inSurface.GetExtent();
                    mBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), Extent));
          }

      private:
          const vk::Device& mDevice;
          vk::CommandBuffer mBuffer;
};

} // namespace ksai
