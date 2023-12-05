#include "VulkanQueue.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanQueueContext.hpp"
using namespace ksai;

ksai::VulkanQueueBase::VulkanQueueBase(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice) 
	: mDevice(inDevice.GetDeviceHandle()),
	mQueueContext(inQueueContext)
{
}

using namespace ksai;

/* Color Attachment */
template<>
template<>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::ColorAttachment>(
    const VulkanSemaphore &inImageAvailableSemaphore,
    const VulkanSemaphore &inRenderFinishedSemaphore,
    const VulkanFence &inFlightFence,
    const VulkanCommandBuffer &inCommandBuffer) const
{
    vk::PipelineStageFlags WaitFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo info;
    info = vk::SubmitInfo(inImageAvailableSemaphore.GetSemaphoreHandle(),
                          WaitFlags,
                          inCommandBuffer.GetCommandBufferHandle(),
                          inRenderFinishedSemaphore.GetSemaphoreHandle());
    mQueue.submit(info);
    mQueue.waitIdle();
}

/* Single Time*/
template<>
template<>
void VulkanQueue<QueueContext::Graphics>::Submit<SubmitContext::SingleTime>(
    const VulkanCommandBuffer &inCommandBuffer) const
{
    vk::SubmitInfo SubmitInfo({}, {}, inCommandBuffer.GetCommandBufferHandle(), {});
    mQueue.submit(SubmitInfo);
    mQueue.waitIdle();
}
