#include "VulkanCommandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

using namespace ksai;
using vb = VulkanCommandBuffer;

vb::VulkanCommandBuffer(const VulkanDevice &inDevice, const VulkanCommandPool &inPool, vb::Type inContext) {
    Init({&inDevice, &inPool, inContext});
}

void VulkanCommandBuffer::Init(CreateInfo inCreateInfo) {
    mDevice = &inCreateInfo.inDevice->GetDeviceHandle();
    vk::CommandBufferAllocateInfo CommandBufferAllocateInfo;
    if (inCreateInfo.inContext == Type::Primary) {
        CommandBufferAllocateInfo =
             vk::CommandBufferAllocateInfo(inCreateInfo.inPool->GetCommandPoolHandle(), vk::CommandBufferLevel::ePrimary, 1);
    } else {
        CommandBufferAllocateInfo =
             vk::CommandBufferAllocateInfo(inCreateInfo.inPool->GetCommandPoolHandle(), vk::CommandBufferLevel::eSecondary, 1);
    }

    mBuffer      = mDevice->allocateCommandBuffers(CommandBufferAllocateInfo).front();
    mInitialized = true;
}
void VulkanCommandBuffer::Destroy() { mInitialized = false; }

vb::~VulkanCommandBuffer() {
    if (mInitialized) {
        Destroy();
    }
}

template <>
void vb::BeginRenderPass<vb::RenderPassBeginContext::Inline>(const VulkanRenderPassBase &inRenderPass,
                                                             const vk::Extent2D inExtent,
                                                             const VulkanFrameBufferBase &inFrameBuffer,
                                                             std::array<float, 5> inClearValue,
                                                             int inClearValueCount) const {
    std::vector<vk::ClearValue> ClearValues;
    ClearValues.resize(inClearValueCount,
                       vk::ClearColorValue(inClearValue[0], inClearValue[1], inClearValue[2], inClearValue[3]));
    ClearValues[inClearValueCount - 1] = vk::ClearDepthStencilValue(inClearValue[4]);
    auto RenderPassBeginInfo           = vk::RenderPassBeginInfo()
                                    .setRenderPass(inRenderPass.GetRenderPassHandle())
                                    .setFramebuffer(inFrameBuffer.GetFrameBufferHandle())
                                    .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), inExtent))
                                    .setClearValues(ClearValues);

    mBuffer.beginRenderPass(RenderPassBeginInfo, vk::SubpassContents::eInline);
    mBuffer.setViewport(
         0, vk::Viewport(0.0f, 0.0f, static_cast<float>(inExtent.width), static_cast<float>(inExtent.height), 0.0f, 1.0f));
    mBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), inExtent));
}

template <>
void vb::BeginRenderPass<vb::RenderPassBeginContext::SecondaryCommandBuffers>(const VulkanRenderPassBase &inRenderPass,
                                                                              const vk::Extent2D inExtent,
                                                                              const VulkanFrameBufferBase &inFrameBuffer,
                                                                              std::array<float, 5> inClearValue,
                                                                              int inClearValueCount) const {

    std::vector<vk::ClearValue> ClearValues;
    ClearValues.resize(inClearValueCount,
                       vk::ClearColorValue(inClearValue[0], inClearValue[1], inClearValue[2], inClearValue[3]));
    ClearValues[inClearValueCount - 1] = vk::ClearDepthStencilValue(inClearValue[4]);
    auto RenderPassBeginInfo           = vk::RenderPassBeginInfo()
                                    .setRenderPass(inRenderPass.GetRenderPassHandle())
                                    .setFramebuffer(inFrameBuffer.GetFrameBufferHandle())
                                    .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), inExtent))
                                    .setClearValueCount(ClearValues.size())
                                    .setClearValues(ClearValues);

    mBuffer.beginRenderPass(RenderPassBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);
}

void vb::EndRenderPass() const { mBuffer.endRenderPass(); }

template <> void vb::Begin<vb::BeginContext::Normal>() const { mBuffer.begin(vk::CommandBufferBeginInfo()); }

template <> void vb::Begin<vb::BeginContext::ContinueRenderPass>() const {
    mBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue));
}

template <>
void vb::Begin<vb::BeginContext::ContinueRenderPass>(VulkanRenderPassBase &inRenderPass,
                                                     ui inSubpass,
                                                     VulkanFrameBufferBase &inFrameBuffer) const {
    vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eRenderPassContinue);
    vk::CommandBufferInheritanceInfo inheritanceInfo(
         inRenderPass.GetRenderPassHandle(), inSubpass, inFrameBuffer.GetFrameBufferHandle());
    info.setPInheritanceInfo(&inheritanceInfo);
    mBuffer.begin(info);
}

template <>
void vb::Begin<vb::BeginContext::ContinueRenderPassAndOneTimeSubmit>(VulkanRenderPassBase &inRenderPass,
                                                                     ui inSubpass,
                                                                     VulkanFrameBufferBase &inFrameBuffer) const {
    vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eRenderPassContinue |
                                    vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    vk::CommandBufferInheritanceInfo inheritanceInfo(
         inRenderPass.GetRenderPassHandle(), inSubpass, inFrameBuffer.GetFrameBufferHandle());
    info.setPInheritanceInfo(&inheritanceInfo);
    mBuffer.begin(info);
}
