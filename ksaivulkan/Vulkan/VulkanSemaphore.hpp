#pragma once
#include "VulkanDevice.hpp"
namespace ksai {

class VulkanSemaphore {
    public:
    operator vk::Semaphore() const { return mSemaphore; }
    VulkanSemaphore(const VulkanDevice& inDevice);
    ~VulkanSemaphore();
    GETTER& GetSemaphoreHandle() const { return mSemaphore; }

    private:
    vk::Semaphore mSemaphore;
    const vk::Device& mDevice;
};

} // namespace ksai
