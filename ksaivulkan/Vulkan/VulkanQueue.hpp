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
    VulkanQueueBase(const VulkanQueueContext& inQueueContext, const VulkanDevice& inDevice);
    void Wait() const;
    GETTER& GetQueueHandle() const { return mQueue; }
    GETTER& GetQueueContext() { return mQueueContext; }

    protected:
    const vk::Device& mDevice;
    const VulkanQueueContext& mQueueContext;
    vk::Queue mQueue;
};

template <QueueContext inContext> class VulkanQueue : public VulkanQueueBase {
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

    template <SubmitContext inSubmitContext>
    void Submit(const VulkanCommandBuffer& inCommandBuffer, const VulkanFence& inFence) const;

    template <SubmitContext inSubmitContext>
    ui Present(const VulkanSwapChain& inSwapChain,
               const VulkanSemaphore& inRenderFinishedSemaphore,
               ui inImageIndex) const;
};

template <QueueContext inContext>
inline VulkanQueue<inContext>::VulkanQueue(const VulkanQueueContext& inQueueContext,
                                           const VulkanDevice& inDevice)
    : VulkanQueueBase(inQueueContext, inDevice) {}

template <QueueContext inContext> inline VulkanQueue<inContext>::~VulkanQueue() {}

template <>
inline VulkanQueue<QueueContext::Graphics>::VulkanQueue(const VulkanQueueContext& inQueueContext,
                                                        const VulkanDevice& inDevice)
    : VulkanQueueBase(inQueueContext, inDevice) {
    mQueue = mDevice.getQueue(inQueueContext.GetGraphicsQueueFamilyIndex(), 0);
}

} // namespace ksai