#include "VulkanSemaphore.hpp"

using namespace ksai;

VulkanSemaphore::VulkanSemaphore(VulkanDevice &inDevice) { Init({&inDevice}); }

VulkanSemaphore::~VulkanSemaphore() {
    if (mInitialized) {
        Destroy();
    }
}

void VulkanSemaphore::Init(CreateInfo inCreateInfo) {
    mDevice                  = &inCreateInfo.mDevice->GetDeviceHandle();
    auto SemaphoreCreateInfo = vk::SemaphoreCreateInfo(vk::SemaphoreCreateFlags());
    mSemaphore               = mDevice->createSemaphore(SemaphoreCreateInfo);
    mInitialized             = true;
}
void VulkanSemaphore::Destroy() {
    if (mSemaphore) {
        mDevice->destroySemaphore(mSemaphore);
    }
}