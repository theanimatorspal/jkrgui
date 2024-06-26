#include "VulkanQueue.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSwapChain.hpp"

using namespace ksai;
static std::mutex SubmitMutex;

void ksai::VulkanQueueBase::Wait() const {
    std::lock_guard<std::mutex> Lock(SubmitMutex);
    mQueue.waitIdle();
}

ksai::VulkanQueueBase::VulkanQueueBase(const VulkanQueueContext& inQueueContext,
                                       const VulkanDevice& inDevice)
    : mDevice(inDevice.GetDeviceHandle()), mQueueContext(inQueueContext) {}

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
    std::lock_guard<std::mutex> Lock(SubmitMutex);
    mQueue.submit(info, inFlightFence.GetFenceHandle());
}

/* Single Time*/
template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
     const VulkanCommandBuffer& inCommandBuffer) const {
    vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    std::lock_guard<std::mutex> Lock(SubmitMutex);
    mQueue.submit(SubmitInfo);
    mQueue.waitIdle();
}

template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
     const VulkanCommandBuffer& inCommandBuffer, const VulkanFence& inFence) const {
    vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    std::lock_guard<std::mutex> Lock(SubmitMutex);
    mQueue.submit(SubmitInfo, inFence.GetFenceHandle());
    // mQueue.waitIdle();
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
    std::lock_guard<std::mutex> Lock(SubmitMutex);
    vk::Result Result = mQueue.presentKHR(PresentInfoKHR);
    return static_cast<ui>(Result);
}