#pragma once
#include "VulkanDevice.hpp"
namespace ksai {

class VulkanSemaphore {
    public:
    struct CreateInfo {
        VulkanDevice *mDevice;
    };

    VulkanSemaphore() = default;
    ~VulkanSemaphore();
    VulkanSemaphore(VulkanSemaphore &other)             = delete;
    VulkanSemaphore &operator=(VulkanSemaphore &other)  = delete;
    VulkanSemaphore(VulkanSemaphore &&other)            = default;
    VulkanSemaphore &operator=(VulkanSemaphore &&other) = default;
    operator vk::Semaphore() const { return mSemaphore; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    GETTER &GetSemaphoreHandle() const { return mSemaphore; }

    VulkanSemaphore(VulkanDevice &inDevice);

    private:
    vk::Semaphore mSemaphore;
    vk::Device *mDevice;
    bool mInitialized = false;
};

} // namespace ksai
