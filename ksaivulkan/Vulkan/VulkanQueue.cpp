#include "VulkanQueue.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSwapChain.hpp"
using namespace ksai;

ksai::VulkanQueueBase::VulkanQueueBase(const VulkanQueueContext& inQueueContext,
                                       const VulkanDevice& inDevice)
    : mDevice(inDevice.GetDeviceHandle()), mQueueContext(inQueueContext) {}

using namespace ksai;

/* Color Attachment */
template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::ColorAttachment>(
     const VulkanSemaphore& inImageAvailableSemaphore,
     const VulkanSemaphore& inRenderFinishedSemaphore,
     const VulkanFence& inFlightFence,
     const VulkanCommandBuffer& inCommandBuffer) const {
    vk::PipelineStageFlags WaitFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo info;
    info = vk::SubmitInfo(inImageAvailableSemaphore.GetSemaphoreHandle(),
                          WaitFlags,
                          inCommandBuffer.GetCommandBufferHandle(),
                          inRenderFinishedSemaphore.GetSemaphoreHandle());
    mQueue.submit(info, inFlightFence.GetFenceHandle());
}

/* Single Time*/
template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
     const VulkanCommandBuffer& inCommandBuffer) const {
    vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    mQueue.submit(SubmitInfo);
    mQueue.waitIdle();
}

template <>
template <>
ui VulkanQueue<QueueContext::Graphics>::Present<SubmitContext::ColorAttachment>(
     const VulkanSwapChain& inSwapChain,
     const VulkanSemaphore& inRenderFinishedSemaphore,
     ui inImageIndex) const {
    auto PresentInfoKHR = vk::PresentInfoKHR(inRenderFinishedSemaphore.GetSemaphoreHandle(),
                                             inSwapChain.GetSwapChainHandle(),
                                             inImageIndex);
    vk::Result Result   = mQueue.presentKHR(PresentInfoKHR);
    return static_cast<ui>(Result);
}