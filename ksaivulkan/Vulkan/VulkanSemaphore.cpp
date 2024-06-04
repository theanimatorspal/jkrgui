#include "VulkanSemaphore.hpp"

using namespace ksai;

VulkanSemaphore::VulkanSemaphore(const VulkanDevice& inDevice)
    : mDevice(inDevice.GetDeviceHandle()) {
    auto SemaphoreCreateInfo = vk::SemaphoreCreateInfo(vk::SemaphoreCreateFlags());
    mSemaphore               = mDevice.createSemaphore(SemaphoreCreateInfo);
}

VulkanSemaphore::~VulkanSemaphore() { mDevice.destroySemaphore(mSemaphore); }
