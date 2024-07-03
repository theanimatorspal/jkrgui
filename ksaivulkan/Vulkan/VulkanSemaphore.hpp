#pragma once
#include "VulkanDevice.hpp"
namespace ksai {

class VulkanSemaphore {
    public:
    struct CreateInfo {
        const VulkanDevice *mDevice;
    };

    VulkanSemaphore() = default;
    ~VulkanSemaphore();
    VulkanSemaphore(const VulkanSemaphore &other)            = delete;
    VulkanSemaphore &operator=(const VulkanSemaphore &other) = delete;
    VulkanSemaphore(VulkanSemaphore &&other)                 = default;
    VulkanSemaphore &operator=(VulkanSemaphore &&other)      = default;
    operator vk::Semaphore() const { return mSemaphore; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    GETTER &GetSemaphoreHandle() const { return mSemaphore; }

    VulkanSemaphore(const VulkanDevice &inDevice);

    private:
    vk::Semaphore mSemaphore;
    const vk::Device *mDevice;
    bool mInitialized = false;
};

} // namespace ksai
