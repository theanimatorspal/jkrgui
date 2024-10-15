#pragma once

#include "VulkanInstance.hpp"

namespace ksai {
enum class MemoryType { DeviceLocal, HostVisibleAndCoherenet, HostVisibleAndHostCached };
}

namespace ksai {

class VulkanPhysicalDevice {
    public:
    operator vk::PhysicalDevice() const { return mPhysicalDevice; }
    VulkanPhysicalDevice(const VulkanInstance &inInstance);
    ~VulkanPhysicalDevice();
    GETTER &GetPhysicalDeviceHandle() const { return mPhysicalDevice; }
    GETTER &GetVulkanInstance() const { return *mVulkanInstance; }

    private:
    const vk::Instance *mInstance = nullptr;
    const VulkanInstance *mVulkanInstance;
    vk::PhysicalDevice mPhysicalDevice;
    vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
    vk::SampleCountFlags mMaxSampleCount;
};
} // namespace ksai
