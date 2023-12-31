#pragma once
#include "VulkanDevice.hpp"
#include "VulkanFence.hpp"
#include "VulkanQueueContext.hpp"
#include "VulkanSemaphore.hpp"

namespace ksai {
class VulkanQueueBase {
public:
    VulkanQueueBase(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice);
    void Wait() const { mQueue.waitIdle(); }

protected:
    const vk::Device& mDevice;
    const VulkanQueueContext& mQueueContext;
    vk::Queue mQueue;
};
}

namespace ksai {
class VulkanCommandBuffer;
template <sz inMaxFramesFlight>
class VulkanSwapChain;

template <QueueContext inContext>
class VulkanQueue : public VulkanQueueBase {
public:
    VulkanQueue(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice);
    ~VulkanQueue();

    template <SubmitContext inSubmitContext>
    void Submit(const VulkanSemaphore& inImageAvailableSemaphore,
        const VulkanSemaphore& inRenderFinishedSemaphore,
        const VulkanFence& inFlightFence,
        const VulkanCommandBuffer& inCommandBuffer) const;

    template <SubmitContext inSubmitContext>
    void Submit(const VulkanCommandBuffer& inCommandBuffer) const;

    template <sz inMaxFramesInFlight, SubmitContext inSubmitContext>
    ui Present(const VulkanSwapChain<inMaxFramesInFlight>& inSwapChain,
        const VulkanSemaphore& inRenderFinishedSemaphore,
        ui inImageIndex) const;
};
}

namespace ksai {

template <QueueContext inContext>
inline VulkanQueue<inContext>::VulkanQueue(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice)
    : VulkanQueueBase(inQueueContext, inDevice)
{
}

template <QueueContext inContext>
inline VulkanQueue<inContext>::~VulkanQueue()
{
}

template <>
inline VulkanQueue<QueueContext::Graphics>::VulkanQueue(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice)
    : VulkanQueueBase(inQueueContext, inDevice)
{
    mQueue = mDevice.getQueue(inQueueContext.GetGraphicsQueueFamilyIndex(), 0);
}

template <QueueContext inContext>
template <sz inMaxFramesInFlight, SubmitContext inSubmitContext>
inline ui VulkanQueue<inContext>::Present(
    const VulkanSwapChain<inMaxFramesInFlight>& inSwapChain,
    const VulkanSemaphore& inRenderFinishedSemaphore,
    ui inImageIndex) const
{
    return 0;
}

template <>
template <sz inMaxFramesInFlight, SubmitContext inSubmitContext>
inline ui VulkanQueue<QueueContext::Graphics>::Present(
    const VulkanSwapChain<inMaxFramesInFlight>& inSwapChain,
    const VulkanSemaphore& inRenderFinishedSemaphore,
    ui inImageIndex) const
{
    auto PresentInfoKHR = vk::PresentInfoKHR(inRenderFinishedSemaphore.GetSemaphoreHandle(), inSwapChain.GetSwapChainHandle(), inImageIndex);
    vk::Result Result = mQueue.presentKHR(PresentInfoKHR);
    return static_cast<ui>(Result);
}
}
