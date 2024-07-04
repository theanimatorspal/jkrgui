#pragma once
#include "VulkanDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

class VulkanCommandPool {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice;
        const VulkanQueueContext *inQueueContext;
    };

    VulkanCommandPool() = default;
    ~VulkanCommandPool();
    VulkanCommandPool(const VulkanCommandPool &other)            = delete;
    VulkanCommandPool &operator=(const VulkanCommandPool &other) = delete;
    VulkanCommandPool(VulkanCommandPool &&other)                 = default;
    VulkanCommandPool &operator=(VulkanCommandPool &&other)      = default;
    operator vk::CommandPool() const { return mPool; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanCommandPool(const VulkanDevice &inDevice, const VulkanQueueContext &inContext);
    GETTER &GetCommandPoolHandle() const { return mPool; }
    void Reset() const { mDevice->resetCommandPool(mPool); }

    private:
    const vk::Device *mDevice;
    vk::CommandPool mPool;
    bool mInitialized = false;
};

} // namespace ksai
