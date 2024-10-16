#pragma once

#include "VulkanInstance.hpp"

namespace ksai {
enum class MemoryType { DeviceLocal, HostVisibleAndCoherenet, HostVisibleAndHostCached };
}

namespace ksai {

class VulkanPhysicalDevice {
    public:
    operator vk::PhysicalDevice() { return mPhysicalDevice; }
    VulkanPhysicalDevice(VulkanInstance &inInstance);
    ~VulkanPhysicalDevice();
    GETTER &GetPhysicalDeviceHandle() { return mPhysicalDevice; }
    GETTER &GetVulkanInstance() { return *mVulkanInstance; }

    private:
    vk::Instance *mInstance = nullptr;
    VulkanInstance *mVulkanInstance;
    vk::PhysicalDevice mPhysicalDevice;
    vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
    vk::SampleCountFlags mMaxSampleCount;
};
} // namespace ksai
