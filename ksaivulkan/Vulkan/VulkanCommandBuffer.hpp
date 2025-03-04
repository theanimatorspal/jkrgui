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
    enum class BeginContext {
        Normal,
        ContinueRenderPass,
        OneTimeSubmit,
        ContinueRenderPassAndOneTimeSubmit
    };

    struct CreateInfo {
        VulkanDevice *inDevice    = nullptr;
        VulkanCommandPool *inPool = nullptr;
        Type inContext            = Type::Primary;
    };

    VulkanCommandBuffer() = default;
    ~VulkanCommandBuffer();
    VulkanCommandBuffer(VulkanCommandBuffer &other)             = delete;
    VulkanCommandBuffer &operator=(VulkanCommandBuffer &other)  = delete;
    VulkanCommandBuffer(VulkanCommandBuffer &&other)            = default;
    VulkanCommandBuffer &operator=(VulkanCommandBuffer &&other) = default;
    operator vk::CommandBuffer() { return mBuffer; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanCommandBuffer(VulkanDevice &inDevice,
                        VulkanCommandPool &inPool,
                        Type inContext = Type::Primary);
    GETTER &GetCommandBufferHandle() { return mBuffer; }
    template <BeginContext inContext = BeginContext::Normal> void Begin();

    template <BeginContext inContext = BeginContext::Normal>
    void Begin(VulkanRenderPassBase &inRenderPass,
               ksai::ui inSubpass,
               VulkanFrameBufferBase &inFrameBuffer);
    void Reset() { mBuffer.reset(); }
    void End() { mBuffer.end(); }
    void ExecuteCommands(VulkanCommandBuffer &inBuffer) {
        mBuffer.executeCommands(inBuffer.mBuffer);
    }

    enum class RenderPassBeginContext { Inline, SecondaryCommandBuffers };
    template <RenderPassBeginContext inBeginContext = RenderPassBeginContext::Inline>
    void BeginRenderPass(VulkanRenderPassBase &inRenderPass,
                         vk::Extent2D inSurface,
                         VulkanFrameBufferBase &inFrameBuffer,
                         std::array<float, 5> inClearValue,
                         int inClearValueCount = 2);

    void EndRenderPass();
    template <class T>
    void PushConstants(VulkanPipelineLayoutBase &inPipelineLayout, vk::ArrayProxy<T> inValues) {
        mBuffer.pushConstants<T>(inPipelineLayout.GetPipelineLayoutHandle(),
                                 vk::ShaderStageFlagBits::eVertex |
                                      vk::ShaderStageFlagBits::eFragment |
                                      vk::ShaderStageFlagBits::eCompute,
                                 0,
                                 inValues);
    }
    void SetViewport(VulkanSurface &inSurface) {
        auto &Extent = inSurface.GetExtent();
        mBuffer.setViewport(0,
                            vk::Viewport(0.0f,
                                         0.0f,
                                         static_cast<float>(Extent.width),
                                         static_cast<float>(Extent.height),
                                         0.0f,
                                         1.0f));
    }
    void SetScissor(VulkanSurface &inSurface) {
        auto &Extent = inSurface.GetExtent();
        mBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), Extent));
    }

    private:
    vk::Device *mDevice;
    vk::CommandBuffer mBuffer;
    bool mInitialized = false;
};

template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
    VulkanCommandBuffer &inCommandBuffer);

template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
    VulkanCommandBuffer &inCommandBuffer, VulkanFence &inFence);

template <> void VulkanCommandBuffer::Begin<VulkanCommandBuffer::BeginContext::Normal>();
template <> void VulkanCommandBuffer::Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPass>();

} // namespace ksai
