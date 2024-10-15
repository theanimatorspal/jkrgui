#pragma once
#include "VulkanPhysicalDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

enum class VulkanDeviceFeatureSet { Default, Minimal, Extensive };

class VulkanDevice {
    public:
    struct CreateInfo {
        const VulkanPhysicalDevice *mPhysicalDevice = nullptr;
        const VulkanQueueContext *mQueueContext     = nullptr;
        VulkanDeviceFeatureSet mFeatureSet          = VulkanDeviceFeatureSet::Extensive;
    };

    VulkanDevice() = default;
    ~VulkanDevice();
    VulkanDevice(const VulkanDevice &other)            = delete;
    VulkanDevice &operator=(const VulkanDevice &other) = delete;
    VulkanDevice(VulkanDevice &&other)                 = default;
    VulkanDevice &operator=(VulkanDevice &&other)      = default;
    operator vk::Device() const { return mDevice; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanDevice(const VulkanPhysicalDevice &inPhysicalDevice,
                 const VulkanQueueContext &inQueueContext,
                 VulkanDeviceFeatureSet inFeatureSet = VulkanDeviceFeatureSet::Extensive);
    void Wait() const;
    GETTER &GetDeviceHandle() const { return mDevice; }
    GETTER &GetPhysicalDeviceHandle() const { return *mPhysicalDevice; }
    GETTER &GetVulkanInstance() const { return mVulkanPhysicalDevice->GetVulkanInstance(); }

    private:
    std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
    const vk::PhysicalDevice *mPhysicalDevice;
    const VulkanPhysicalDevice *mVulkanPhysicalDevice;
    vk::Device mDevice = nullptr;
    bool mInitialized  = false;
};

} // namespace ksai
