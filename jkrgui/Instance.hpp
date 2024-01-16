#pragma once
#include "ksai_thread.hpp"
#include "Global/Standards.hpp"
#include "TestUtils.hpp"
#include <Vulkan/Pipeline/VulkanDescriptorPool.hpp>
#include <Vulkan/Pipeline/VulkanDescriptorSet.hpp>
#include <Vulkan/Pipeline/VulkanDescriptorUpdateHandler.hpp>
#include <Vulkan/Pipeline/VulkanPipeline.hpp>
#include <Vulkan/VulkanBufferVMA.hpp>
#include <Vulkan/VulkanImageVMA.hpp>
#include <Vulkan/VulkanSampler.hpp>
#include <VulkanCommandBuffer.hpp>
#include <VulkanCommandPool.hpp>
#include <VulkanDevice.hpp>
#include <VulkanFence.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanImage.hpp>
#include <VulkanInstance.hpp>
#include <VulkanMessenger.hpp>
#include <VulkanPhysicalDevice.hpp>
#include <VulkanQueue.hpp>
#include <VulkanQueueContext.hpp>
#include <VulkanRenderPass.hpp>
#include <VulkanSurface.hpp>
#include <VulkanSwapChain.hpp>
#include <chrono>

namespace Jkr {
using namespace ksai;
static Timer time;
class Instance {
public:
    Instance(uint32_t inNoOfDescriptors = 20, uint32_t inPoolSize = 10000);
    GETTER& GetVulkanInstance() const { return mInstance; }
    GETTER& GetPhysicalDevice() const { return mPhysicalDevice; }
    GETTER& GetQueueContext() const { return mQueueContext; }
    GETTER& GetDevice() const { return mDevice; }

    GETTER& GetGraphicsQueue() const { return mGraphicsQueue; }
    GETTER& GetCommandPool() const { return mCommandPool; }
    // GETTER& GetCommandBuffers() const { return mCommandBuffers; }
    GETTER& GetUtilCommandBuffer() const { return mUtilCommandBuffer; }
    GETTER& GetDescriptorPool() const { return mDescriptorPool; }

    GETTER& GetVMA() const { return mVmaAllocator; }

    GETTER& GetThreadPool() { return mThreadPool; }

private:
    static const int mMaxFramesInFlight = 2;
private:
    const VulkanInstance mInstance;
    const VulkanMessenger mMessenger;
    const VulkanSurface mSurface;
    const VulkanPhysicalDevice mPhysicalDevice;
    const VulkanQueueContext mQueueContext;
    const VulkanDevice mDevice;
    const VulkanQueue<QueueContext::Graphics> mGraphicsQueue;
    const VulkanCommandPool mCommandPool;
    // const std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
    const VulkanCommandBuffer mUtilCommandBuffer;
    const VulkanDescriptorPool mDescriptorPool;
    const VulkanVMA mVmaAllocator;
    ksai::ThreadPool mThreadPool;
};
}
