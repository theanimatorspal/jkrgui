#include "Instance.hpp"
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

using namespace ksai;

Jkr::Instance::Instance(uint32_t inNoOfDescriptors, uint32_t inPoolSize, bool inEnableValidation) {
    SpirvHelper::Init();
    mThreadPool.SetThreadCount(std::thread::hardware_concurrency() - 1);
    mInstance = MakeUp<VulkanInstance>(inEnableValidation);
    if (inEnableValidation) {
        mMessenger = MakeUp<VulkanMessenger>(*mInstance);
    }
    mPhysicalDevice         = MakeUp<VulkanPhysicalDevice>(*mInstance);
    mQueueContext           = MakeUp<VulkanQueueContext>(*mInstance, *mPhysicalDevice);
    mDevice                 = MakeUp<VulkanDevice>(*mPhysicalDevice, *mQueueContext);
    mGraphicsPresentQueue   = MakeUp<VulkanQueue<QueueContext::Graphics>>(*mQueueContext, *mDevice);
    mDescriptorPool         = MakeUp<VulkanDescriptorPool>(*mDevice, inNoOfDescriptors, inPoolSize);
    mVmaAllocator           = MakeUp<VulkanVMA>(*mInstance, *mDevice);
    mCommandPool            = MakeUp<VulkanCommandPool>(*mDevice, *mQueueContext);
    mUtilCommandBuffer      = MakeUp<VulkanCommandBuffer>(*mDevice, *mCommandPool);
    mUtilCommandBufferFence = MakeUp<VulkanFence>(*mDevice);
}

Jkr::Instance::~Instance() { SpirvHelper::Finalize(); }

std::mutex StagingBufferAccessMutex;
VulkanBufferVMA &Jkr::Instance::GetStagingBuffer(size_t inSize) {
    std::scoped_lock<std::mutex> StagingBufferAccessLock(StagingBufferAccessMutex);
    if ((not mVulkanStagingBufferPool) or mVulkanStagingBufferPool->GetBufferSize() <= inSize) {
        mVulkanStagingBufferPool = MakeUp<VulkanBufferVMA>();
        mVulkanStagingBufferPool->Init(
             VulkanBufferVMA::CreateInfo{mVmaAllocator.get(),
                                         mDevice.get(),
                                         inSize,
                                         BufferContext::Staging,
                                         MemoryType::HostVisibleAndCoherenet});
    }
    void *data;
    mVulkanStagingBufferPool->MapMemoryRegion(&data);
    std::memset(data, 0, mVulkanStagingBufferPool->GetBufferSize());
    return *mVulkanStagingBufferPool;
}