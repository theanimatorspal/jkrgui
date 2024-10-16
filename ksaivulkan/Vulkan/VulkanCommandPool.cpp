#include "VulkanCommandPool.hpp"
using namespace ksai;

VulkanCommandPool::VulkanCommandPool(VulkanDevice &inDevice, VulkanQueueContext &inContext) {
    Init({&inDevice, &inContext});
}

VulkanCommandPool::~VulkanCommandPool() {
    if (mInitialized) {
        Destroy();
    }
}

void VulkanCommandPool::Init(CreateInfo info) {
    mDevice = &info.inDevice->GetDeviceHandle();
    auto CommandPoolCreateInfo =
         vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                   info.inQueueContext->GetGraphicsQueueFamilyIndex());
    mPool        = mDevice->createCommandPool(CommandPoolCreateInfo);
    mInitialized = true;
}

void VulkanCommandPool::Destroy() {
    mDevice->waitIdle();
    mDevice->destroyCommandPool(mPool);
    mInitialized = false;
}