#pragma once
#include "VulkanDevice.hpp"
#include "VulkanFence.hpp"
#include "VulkanQueueContext.hpp"
#include "VulkanSemaphore.hpp"

namespace ksai {
class VulkanCommandBuffer;
class VulkanSwapChain;
class VulkanQueueBase {
    public:
    operator vk::Queue() { return mQueue; }
    VulkanQueueBase(VulkanQueueContext &inQueueContext, VulkanDevice &inDevice);
    void Wait();
    GETTER &GetQueueHandle() { return mQueue; }
    GETTER &GetQueueContext() { return mQueueContext; }

    protected:
    vk::Device &mDevice;
    VulkanQueueContext &mQueueContext;
    vk::Queue mQueue;
};

template <QueueContext inContext> class VulkanQueue : public VulkanQueueBase {
    public:
    VulkanQueue(VulkanQueueContext &inQueueContext, VulkanDevice &inDevice);
    ~VulkanQueue();

    template <SubmitContext inSubmitContext>
    void Submit(VulkanSemaphore &inImageAvailableSemaphore,
                VulkanSemaphore &inRenderFinishedSemaphore,
                VulkanFence &inFlightFence,
                VulkanCommandBuffer &inCommandBuffer);

    template <SubmitContext inSubmitContext> void Submit(VulkanCommandBuffer &inCommandBuffer);

    template <SubmitContext inSubmitContext>
    void Submit(VulkanCommandBuffer &inCommandBuffer, VulkanFence &inFence);

    template <SubmitContext inSubmitContext>
    ui Present(VulkanSwapChain &inSwapChain,
               VulkanSemaphore &inRenderFinishedSemaphore,
               ui inImageIndex);
};

template <QueueContext inContext>
inline VulkanQueue<inContext>::VulkanQueue(VulkanQueueContext &inQueueContext,
                                           VulkanDevice &inDevice)
    : VulkanQueueBase(inQueueContext, inDevice) {}

template <QueueContext inContext> inline VulkanQueue<inContext>::~VulkanQueue() {}

template <>
inline VulkanQueue<QueueContext::Graphics>::VulkanQueue(VulkanQueueContext &inQueueContext,
                                                        VulkanDevice &inDevice)
    : VulkanQueueBase(inQueueContext, inDevice) {
    mQueue = mDevice.getQueue(inQueueContext.GetGraphicsQueueFamilyIndex(), 0);
}

} // namespace ksai