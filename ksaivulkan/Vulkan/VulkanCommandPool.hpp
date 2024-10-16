#pragma once
#include "VulkanDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

class VulkanCommandPool {
    public:
    struct CreateInfo {
        VulkanDevice *inDevice;
        VulkanQueueContext *inQueueContext;
    };

    VulkanCommandPool() = default;
    ~VulkanCommandPool();
    VulkanCommandPool(VulkanCommandPool &other)             = delete;
    VulkanCommandPool &operator=(VulkanCommandPool &other)  = delete;
    VulkanCommandPool(VulkanCommandPool &&other)            = default;
    VulkanCommandPool &operator=(VulkanCommandPool &&other) = default;
    operator vk::CommandPool() const { return mPool; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanCommandPool(VulkanDevice &inDevice, VulkanQueueContext &inContext);
    GETTER &GetCommandPoolHandle() { return mPool; }
    void Reset() { mDevice->resetCommandPool(mPool); }

    private:
    vk::Device *mDevice;
    vk::CommandPool mPool;
    bool mInitialized = false;
};

} // namespace ksai
