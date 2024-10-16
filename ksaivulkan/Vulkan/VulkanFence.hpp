#pragma once
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace ksai {

class VulkanFence {
    public:
    struct CreateInfo {
        VulkanDevice *inDevice;
    };

    VulkanFence() = default;
    ~VulkanFence();
    VulkanFence(VulkanFence &other)             = delete;
    VulkanFence &operator=(VulkanFence &other)  = delete;
    VulkanFence(VulkanFence &&other)            = default;
    VulkanFence &operator=(VulkanFence &&other) = default;
    operator vk::Fence() const { return mFence; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanFence(VulkanDevice &inDevice);
    vk::Result Wait() const;
    void Reset() const;
    GETTER &GetFenceHandle() const { return mFence; }

    private:
    vk::Device *mDevice;
    vk::Fence mFence;
    bool mInitialized = false;
};

} // namespace ksai
