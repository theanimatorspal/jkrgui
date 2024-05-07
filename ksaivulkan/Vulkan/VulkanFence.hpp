#pragma once
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace ksai {

class VulkanFence {
    public:
    VulkanFence(const VulkanDevice& inDevice);
    ~VulkanFence();
    vk::Result Wait() const;
    void Reset() const;
    GETTER& GetFenceHandle() const { return mFence; }

    private:
    const vk::Device& mDevice;
    vk::Fence mFence;
};

} // namespace ksai
