#pragma once
#include "VulkanPhysicalDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

enum class VulkanDeviceFeatureSet { Default, Minimal, Extensive };

class VulkanDevice {
    public:
    operator vk::Device() const { return mDevice; }
    VulkanDevice(const VulkanPhysicalDevice& inPhysicalDevice,
                 const VulkanQueueContext& inQueueContext,
                 VulkanDeviceFeatureSet inFeatureSet = VulkanDeviceFeatureSet::Extensive);
    ~VulkanDevice();
    void Wait() const;
    GETTER& GetDeviceHandle() const { return mDevice; }
    GETTER& GetPhysicalDeviceHandle() const { return mPhysicalDevice; }

    private:
    std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
    const vk::PhysicalDevice& mPhysicalDevice;
    vk::Device mDevice;
};

} // namespace ksai
