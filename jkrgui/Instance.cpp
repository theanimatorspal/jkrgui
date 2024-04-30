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

Jkr::Instance::Instance(uint32_t inNoOfDescriptors, uint32_t inPoolSize) {
    mThreadPool.SetThreadCount(std::thread::hardware_concurrency() - 1);
    SpirvHelper::Init();
    mInstance          = MakeUp<VulkanInstance>();
    mMessenger         = MakeUp<VulkanMessenger>(*mInstance);
    mPhysicalDevice    = MakeUp<VulkanPhysicalDevice>(*mInstance);
    mQueueContext      = MakeUp<VulkanQueueContext>(*mInstance, *mPhysicalDevice);
    mDevice            = MakeUp<VulkanDevice>(*mPhysicalDevice, *mQueueContext);
    mGraphicsQueue     = MakeUp<VulkanQueue<QueueContext::Graphics>>(*mQueueContext, *mDevice);
    mDescriptorPool    = MakeUp<VulkanDescriptorPool>(*mDevice, inNoOfDescriptors, inPoolSize);
    mVmaAllocator      = MakeUp<VulkanVMA>(*mInstance, *mDevice);
    mCommandPool       = MakeUp<VulkanCommandPool>(*mDevice, *mQueueContext);
    mUtilCommandBuffer = MakeUp<VulkanCommandBuffer>(*mDevice, *mCommandPool);
}

Jkr::Instance::~Instance() { SpirvHelper::Finalize(); }