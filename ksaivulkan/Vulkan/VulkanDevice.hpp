#pragma once
#include "VulkanPhysicalDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

enum class VulkanDeviceFeatureSet { Default, Minimal, Extensive };

class VulkanDevice {
    public:
    struct CreateInfo {
        VulkanPhysicalDevice *mPhysicalDevice = nullptr;
        VulkanQueueContext *mQueueContext     = nullptr;
        VulkanDeviceFeatureSet mFeatureSet    = VulkanDeviceFeatureSet::Extensive;
    };

    VulkanDevice() = default;
    ~VulkanDevice();
    VulkanDevice(VulkanDevice &other)             = delete;
    VulkanDevice &operator=(VulkanDevice &other)  = delete;
    VulkanDevice(VulkanDevice &&other)            = default;
    VulkanDevice &operator=(VulkanDevice &&other) = default;
    operator vk::Device() const { return mDevice; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanDevice(VulkanPhysicalDevice &inPhysicalDevice,
                 VulkanQueueContext &inQueueContext,
                 VulkanDeviceFeatureSet inFeatureSet = VulkanDeviceFeatureSet::Extensive);
    void Wait();
    GETTER &GetDeviceHandle() { return mDevice; }
    GETTER &GetPhysicalDeviceHandle() { return *mPhysicalDevice; }
    GETTER &GetVulkanInstance() { return mVulkanPhysicalDevice->GetVulkanInstance(); }

    private:
    std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
    vk::PhysicalDevice *mPhysicalDevice;
    VulkanPhysicalDevice *mVulkanPhysicalDevice;
    vk::Device mDevice = nullptr;
    bool mInitialized  = false;
};

} // namespace ksai
