#include "VulkanQueue.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSwapChain.hpp"

using namespace ksai;
static std::mutex SubmitMutex;

void ksai::VulkanQueueBase::Wait() {
    std::scoped_lock<std::mutex> Lock(SubmitMutex);
    mQueue.waitIdle();
}
ksai::VulkanQueueBase::VulkanQueueBase(VulkanQueueContext &inQueueContext, VulkanDevice &inDevice)
    : mDevice(inDevice.GetDeviceHandle()), mQueueContext(inQueueContext) {}

/* Color Attachment */
template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::ColorAttachment>(
     VulkanSemaphore &inImageAvailableSemaphore,
     VulkanSemaphore &inRenderFinishedSemaphore,
     VulkanFence &inFlightFence,
     VulkanCommandBuffer &inCommandBuffer) {
    vk::PipelineStageFlags WaitFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo info;
    info = vk::SubmitInfo(inImageAvailableSemaphore.GetSemaphoreHandle(),
                          WaitFlags,
                          inCommandBuffer.GetCommandBufferHandle(),
                          inRenderFinishedSemaphore.GetSemaphoreHandle());
    std::scoped_lock<std::mutex> Lock(SubmitMutex);
    mQueue.submit(info, inFlightFence.GetFenceHandle());
}

template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::ColorAttachment>(
     VulkanCommandBuffer &inCommandBuffer, VulkanFence &inFlightFence) {
    vk::SubmitInfo info;
    info = vk::SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    std::scoped_lock<std::mutex> Lock(SubmitMutex);
    mQueue.submit(info, inFlightFence.GetFenceHandle());
}

/* Single Time*/
template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
     VulkanCommandBuffer &inCommandBuffer, VulkanFence &inFence) {
    vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    std::scoped_lock<std::mutex> Lock(SubmitMutex);
    mQueue.submit(SubmitInfo, inFence.GetFenceHandle());
}

template <>
template <>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
     VulkanCommandBuffer &inCommandBuffer) {
    vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    std::scoped_lock<std::mutex> Lock(SubmitMutex);
    mQueue.submit(SubmitInfo);
    mQueue.waitIdle();
}

template <>
template <>
ui VulkanQueue<QueueContext::Graphics>::Present<SubmitContext::ColorAttachment>(
     VulkanSwapChain &inSwapChain, VulkanSemaphore &inRenderFinishedSemaphore, ui inImageIndex) {
    auto PresentInfoKHR = vk::PresentInfoKHR(inRenderFinishedSemaphore.GetSemaphoreHandle(),
                                             inSwapChain.GetSwapChainHandle(),
                                             inImageIndex);
    std::scoped_lock<std::mutex> Lock(SubmitMutex);
    vk::Result Result = mQueue.presentKHR(PresentInfoKHR);
    return static_cast<ui>(Result);
}