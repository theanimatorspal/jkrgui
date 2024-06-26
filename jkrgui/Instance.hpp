#pragma once
#include "Global/Standards.hpp"
#include "TestUtils.hpp"
#include "VulkanQueue.hpp"
#include "ksai_thread.hpp"

namespace ksai {
class VulkanInstance;
class VulkanMessenger;
class VulkanPhysicalDevice;
class VulkanQueueContext;
class VulkanDevice;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanDescriptorPool;
class VulkanVMA;
class VulkanFence;
} // namespace ksai

namespace Jkr {
using namespace ksai;
static Timer time;

class Instance {
    public:
    Instance(uint32_t inNoOfDescriptors = 20,
             uint32_t inPoolSize        = 10000,
             bool inEnableValidation    = false);
    ~Instance();
    GETTER& GetVulkanInstance() const { return *mInstance; }
    GETTER& GetPhysicalDevice() const { return *mPhysicalDevice; }
    GETTER& GetQueueContext() const { return *mQueueContext; }
    GETTER& GetDevice() const { return *mDevice; }
    GETTER& GetGraphicsQueue() const { return *mGraphicsPresentQueue; };
    GETTER& GetCommandPool() const { return *mCommandPool; }
    GETTER& GetUtilCommandBuffer() const { return *mUtilCommandBuffer; }
    GETTER& GetUtilCommandBufferFence() const { return *mUtilCommandBufferFence; }
    GETTER& GetDescriptorPool() const { return *mDescriptorPool; }
    GETTER& GetVMA() const { return *mVmaAllocator; }
    GETTER& GetThreadPool() { return mThreadPool; }

    private:
    static const int mMaxFramesInFlight = 2;

    private:
    Up<VulkanInstance> mInstance;
    Up<VulkanMessenger> mMessenger;
    Up<VulkanPhysicalDevice> mPhysicalDevice;
    Up<VulkanQueueContext> mQueueContext;
    Up<VulkanDevice> mDevice;
    Up<VulkanQueue<QueueContext::Graphics>> mGraphicsPresentQueue;
    Up<VulkanCommandPool> mCommandPool;
    Up<VulkanCommandBuffer> mUtilCommandBuffer;
    Up<VulkanFence> mUtilCommandBufferFence;
    Up<VulkanDescriptorPool> mDescriptorPool;
    Up<VulkanVMA> mVmaAllocator;
    ksai::ThreadPool mThreadPool;
};
} // namespace Jkr
