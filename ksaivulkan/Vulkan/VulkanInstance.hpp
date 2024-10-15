#pragma once
#include "Config.hpp"
#include <vulkan/vulkan.hpp>

namespace ksai {
class VulkanInstance {
    public:
    struct CreateInfo {
        bool mEnableValiation = false;
    };
    operator vk::Instance() const { return mInstance; }
    ///@note @todo remove this or complete this
    using DeletionQueueType = std::vector<std::function<void(void)>>;
    GETTER &GetInstanceHandle() const { return mInstance; }
    GETTER IsValidationEnabled() const { return mValidationEnabled; }
    static DeletionQueueType &GetDeletionQueueRef();
    static std::mutex &GetDeletionMutexRef();

    VulkanInstance() = default;
    ~VulkanInstance();
    VulkanInstance(const VulkanInstance &other)            = delete;
    VulkanInstance &operator=(const VulkanInstance &other) = delete;

    VulkanInstance(VulkanInstance &&other)                 = default;
    VulkanInstance &operator=(VulkanInstance &&other)      = default;

    VulkanInstance(bool inEnableValidation = false);
    VulkanInstance(vk::Instance inInstance) : mInstance(inInstance) {}

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    vk::DynamicLoader mDynamicLoader;

    private:
    static DeletionQueueType gDeletionQueue;
    static std::mutex gDeletionMutex;
    vk::Instance mInstance = nullptr;
    v<char const *> mInstanceLayerNames;
    v<char const *> mInstanceExtensionNames;
    static std::mutex mDeletionMutex;
    bool mInitialized       = false;
    bool mValidationEnabled = false;
};
} // namespace ksai
