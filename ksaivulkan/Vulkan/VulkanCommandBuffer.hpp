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

    struct CreateInfo {
        const VulkanDevice *inDevice    = nullptr;
        const VulkanCommandPool *inPool = nullptr;
        Type inContext                  = Type::Primary;
    };

    VulkanCommandBuffer() = default;
    ~VulkanCommandBuffer();
    VulkanCommandBuffer(const VulkanCommandBuffer &other)            = delete;
    VulkanCommandBuffer &operator=(const VulkanCommandBuffer &other) = delete;
    VulkanCommandBuffer(VulkanCommandBuffer &&other)                 = default;
    VulkanCommandBuffer &operator=(VulkanCommandBuffer &&other)      = default;
    operator vk::CommandBuffer() const { return mBuffer; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanCommandBuffer(const VulkanDevice &inDevice, const VulkanCommandPool &inPool, Type inContext = Type::Primary);
    GETTER &GetCommandBufferHandle() const { return mBuffer; }
    template <BeginContext inContext = BeginContext::Normal> void Begin() const;

    template <BeginContext inContext = BeginContext::Normal>
    void Begin(VulkanRenderPassBase &inRenderPass, ksai::ui inSubpass, VulkanFrameBufferBase &inFrameBuffer) const;
    void Reset() const { mBuffer.reset(); }
    void End() const { mBuffer.end(); }
    void ExecuteCommands(const VulkanCommandBuffer &inBuffer) const { mBuffer.executeCommands(inBuffer.mBuffer); }

    enum class RenderPassBeginContext { Inline, SecondaryCommandBuffers };
    template <RenderPassBeginContext inBeginContext = RenderPassBeginContext::Inline>
    void BeginRenderPass(const VulkanRenderPassBase &inRenderPass,
                         const vk::Extent2D inSurface,
                         const VulkanFrameBufferBase &inFrameBuffer,
                         std::array<float, 5> inClearValue,
                         int inClearValueCount = 2) const;

    void EndRenderPass() const;
    template <class T>
    void PushConstants(const VulkanPipelineLayoutBase &inPipelineLayout, const vk::ArrayProxy<const T> inValues) const {
        mBuffer.pushConstants<T>(inPipelineLayout.GetPipelineLayoutHandle(),
                                 vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment |
                                      vk::ShaderStageFlagBits::eCompute,
                                 0,
                                 inValues);
    }
    void SetViewport(const VulkanSurface &inSurface) {
        const auto &Extent = inSurface.GetExtent();
        mBuffer.setViewport(
             0, vk::Viewport(0.0f, 0.0f, static_cast<float>(Extent.width), static_cast<float>(Extent.height), 0.0f, 1.0f));
    }
    void SetScissor(const VulkanSurface &inSurface) {
        const auto &Extent = inSurface.GetExtent();
        mBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), Extent));
    }

    private:
    const vk::Device *mDevice;
    vk::CommandBuffer mBuffer;
    bool mInitialized = false;
};

} // namespace ksai
