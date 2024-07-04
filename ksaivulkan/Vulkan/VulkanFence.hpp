#pragma once
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace ksai {

class VulkanFence {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice;
    };

    VulkanFence() = default;
    ~VulkanFence();
    VulkanFence(const VulkanFence &other)            = delete;
    VulkanFence &operator=(const VulkanFence &other) = delete;
    VulkanFence(VulkanFence &&other)                 = default;
    VulkanFence &operator=(VulkanFence &&other)      = default;
    operator vk::Fence() const { return mFence; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanFence(const VulkanDevice &inDevice);
    vk::Result Wait() const;
    void Reset() const;
    GETTER &GetFenceHandle() const { return mFence; }

    private:
    const vk::Device *mDevice;
    vk::Fence mFence;
    bool mInitialized = false;
};

} // namespace ksai
