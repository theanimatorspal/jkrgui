#pragma once
#include "Config.hpp"
#include <vulkan/vulkan.hpp>
#define CONSTANT constexpr auto
namespace ksai {
class VulkanInstance {
    public:
    operator vk::Instance() const { return mInstance; }
    using DeletionQueueType = std::vector<std::function<void(void)>>;
    GETTER& GetInstanceHandle() const { return mInstance; }
    static DeletionQueueType& GetDeletionQueueRef();
    static std::mutex& GetDeletionMutexRef();
    VulkanInstance(bool inEnableValidation = false);
    VulkanInstance(vk::Instance inInstance) : mInstance(inInstance) {}
    ~VulkanInstance();

    private:
    static DeletionQueueType gDeletionQueue;
    static std::mutex gDeletionMutex;
    vk::Instance mInstance = nullptr;
    v<char const*> mInstanceLayerNames;
    v<char const*> mInstanceExtensionNames;
    static std::mutex mDeletionMutex;
};
} // namespace ksai
