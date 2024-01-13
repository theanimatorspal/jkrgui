#pragma once
#include "VulkanDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

class VulkanCommandPool {
public:
    VulkanCommandPool(const VulkanDevice& inDevice, const VulkanQueueContext& inContext);
    ~VulkanCommandPool();
    GETTER& GetCommandPoolHandle() const { return mPool; }
    void Reset() const { mDevice.resetCommandPool(mPool); }

private:
    const vk::Device& mDevice;
    vk::CommandPool mPool;
};

}
